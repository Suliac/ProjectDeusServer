#include "DeusCore/Logger.h"
#include "DeusCore/Packets.h"

#include "GameNetworkServer.h"
#include "PacketNewPlayer.h"
#include "PacketObjectEnter.h"
#include "PacketObjectLeave.h"

#include <iostream>
namespace DeusServer
{
	GameNetworkServer::GameNetworkServer(int gameId)
	{
		m_gameId = gameId;
		m_name = "Game Server " + std::to_string(gameId);
		DeusCore::Logger::Instance()->Log(m_name, "New " + m_name);

		// Init cells
		for (size_t i = 0; i < NUMBER_CELLS; i++)
		{
			// We start Cells id at 1
			m_cells.insert(std::make_pair(i + 1, std::vector<Id>()));
		}
	}

	//---------------------------------------------------------------------------------
	GameNetworkServer::~GameNetworkServer()
	{
		DeusCore::Logger::Instance()->Log(m_name, "Delete " + m_name);
		if (!m_stopped)
		{
			OnStop();
		}
	}

	/////////////////////////// 
	//         SERVER        // 
	///////////////////////////

	//---------------------------------------------------------------------------------
	void GameNetworkServer::OnStart()
	{
		// create deleguate functions
		DeusCore::DeusEventDeleguate messageInterpretDeleguate = fastdelegate::MakeDelegate(this, &GameNetworkServer::InterpretPacket);

		// register deleguates with connection
		DeusCore::EventManagerHandler::Instance()->AddListener(m_gameId, messageInterpretDeleguate, DeusCore::Packet::EMessageType::Test);
		DeusCore::EventManagerHandler::Instance()->AddListener(m_gameId, messageInterpretDeleguate, DeusCore::Packet::EMessageType::LeaveGameRequest);
		DeusCore::EventManagerHandler::Instance()->AddListener(m_gameId, messageInterpretDeleguate, DeusCore::Packet::EMessageType::NewPlayer);
		DeusCore::EventManagerHandler::Instance()->AddListener(m_gameId, messageInterpretDeleguate, DeusCore::Packet::EMessageType::PlayerReady);

		DeusCore::EventManagerHandler::Instance()->AddListener(m_gameId, messageInterpretDeleguate, DeusCore::Packet::EMessageType::ObjectChangeCell);
	}

	//---------------------------------------------------------------------------------
	void GameNetworkServer::OnStop()
	{
		// create deleguate functions
		DeusCore::DeusEventDeleguate messageInterpretDeleguate = fastdelegate::MakeDelegate(this, &GameNetworkServer::InterpretPacket);

		// register deleguates with connection
		DeusCore::EventManagerHandler::Instance()->RemoveListener(m_gameId, messageInterpretDeleguate, DeusCore::Packet::EMessageType::Test);
		DeusCore::EventManagerHandler::Instance()->RemoveListener(m_gameId, messageInterpretDeleguate, DeusCore::Packet::EMessageType::LeaveGameRequest);
		DeusCore::EventManagerHandler::Instance()->RemoveListener(m_gameId, messageInterpretDeleguate, DeusCore::Packet::EMessageType::NewPlayer);
		DeusCore::EventManagerHandler::Instance()->RemoveListener(m_gameId, messageInterpretDeleguate, DeusCore::Packet::EMessageType::PlayerReady);

		DeusCore::EventManagerHandler::Instance()->RemoveListener(m_gameId, messageInterpretDeleguate, DeusCore::Packet::EMessageType::ObjectChangeCell);
		m_stopped = true;
	}

	//---------------------------------------------------------------------------------
	void GameNetworkServer::OnDisconnectClient(Id clientId)
	{
		// We echo back to the world server the disconnected client id
		DeusCore::PacketSPtr p_disconnectedEvent = std::shared_ptr<DeusCore::PacketClientDisconnect>(new DeusCore::PacketClientDisconnect());
		DeusCore::EventManagerHandler::Instance()->QueueEvent(0, clientId, p_disconnectedEvent);
		DeusCore::Logger::Instance()->Log(m_name, "Yo disconnected ");


		// we check if there is player left, otherwise we stop this game server
		TryDeleteGame();
	}


	/////////////////////////// 
	//          GAMES        // 
	///////////////////////////

	//---------------------------------------------------------------------------------
	void GameNetworkServer::InterpretPacket(DeusCore::DeusEventSPtr p_packet)
	{
		//DeusCore::Logger::Instance()->Log(m_name, "Packet id : " + std::to_string(p_packet->second->GetID()) + ". Sent by client :" + std::to_string(p_packet->first));

		switch (p_packet->second->GetType())
		{
		case DeusCore::Packet::EMessageType::Error:
			DeusCore::Logger::Instance()->Log(m_name, "Try to interpret packet failed. Unknown packet id : " + std::to_string(p_packet->second->GetType()) + ". Sent by client :" + std::to_string(p_packet->first));
			break;

		case DeusCore::Packet::EMessageType::Test:
			DeusCore::Logger::Instance()->Log(m_name, "Client " + std::to_string(p_packet->first) + " : " + ((DeusCore::PacketTest*)p_packet->second.get())->GetTextMessage());
			break;

		case DeusCore::Packet::EMessageType::LeaveGameRequest:
			LeaveGame(p_packet->first);
			break;

		case DeusCore::Packet::EMessageType::NewPlayer:
			NewPlayer(p_packet->first, ((PacketNewPlayer*)p_packet->second.get())->GetConnection());
			break;

		case DeusCore::Packet::EMessageType::PlayerReady:
			PlayerReady(p_packet->first);
			break;

		case DeusCore::Packet::EMessageType::ObjectChangeCell:
			ObjectChangedCell(std::dynamic_pointer_cast<PacketObjectChangeCell>(p_packet->second));
			break;

			//NB : Disconnect event already managed in NetworkServer
		default:
			break;
		}
	}

	//---------------------------------------------------------------------------------
	void GameNetworkServer::LeaveGame(Id clientId)
	{
		m_lockClients.lock(); // <----------------- LOCK
		DeusClientConnections::iterator matchingConnectionIt = m_clientsConnections.find(clientId);
		if (matchingConnectionIt != m_clientsConnections.end())
		{
			matchingConnectionIt->second->SetConnectionGameId(0);
			m_clientsConnections.erase(matchingConnectionIt);
			
			// we check if there is player left, otherwise we stop this game server
			TryDeleteGame();
			DeusCore::Logger::Instance()->Log(m_name, "Client (id:" + std::to_string(clientId) + ") Leaved the game");
		}
		m_lockClients.unlock();// <----------------- UNLOCK
	}

	//---------------------------------------------------------------------------------
	bool GameNetworkServer::NewPlayer(Id clientId, DeusClientSPtr clientConnection)
	{
		m_lockClients.lock();
		DeusClientConnections::iterator findClientId = m_clientsConnections.find(clientId);
		if (findClientId != m_clientsConnections.end())
		{
			m_lockClients.unlock();
			return false; // check if client is already here
		}

		// Insert new client into our map
		m_clientsConnections.insert(std::make_pair(clientId, clientConnection));
		clientConnection->SetConnectionGameId(m_gameId);
		DeusCore::Logger::Instance()->Log(m_name, "Client (id:" + std::to_string(clientId) + ") Joined the game");

		// Init player datas

		m_playersLocker.lock(); // <------------- LOCK
		m_playersInfos[clientId] = DeusPlayerInfos();
		m_playersLocker.unlock(); // <------------- UNLOCK

		// send feedback to client
		std::unique_ptr<DeusCore::PacketJoinGameAnswer> p_feedBackJoinPacket = std::unique_ptr<DeusCore::PacketJoinGameAnswer>(new DeusCore::PacketJoinGameAnswer());
		p_feedBackJoinPacket->SetSuccess(true); 
		p_feedBackJoinPacket->SetGame(clientId);
		m_clientsConnections[clientId]->SendPacket(std::move(p_feedBackJoinPacket), true);
		
		m_lockClients.unlock();
		return true;
	}

	//---------------------------------------------------------------------------------
	void GameNetworkServer::TryDeleteGame()
	{
		m_lockClients.lock(); // <----------------- LOCK
		size_t nbrClients = m_clientsConnections.size();
		m_lockClients.unlock();// <----------------- UNLOCK

		if (nbrClients <= 0)
		{
			RequestStop();
			DeusCore::PacketSPtr p_deleteGameEvent = std::shared_ptr<DeusCore::PacketDeleteGameRequest>(new DeusCore::PacketDeleteGameRequest());
			DeusCore::EventManagerHandler::Instance()->QueueEvent(0, m_gameId, p_deleteGameEvent);
			DeusCore::Logger::Instance()->Log(m_name, "Request game deletion");
		}
	}

	//---------------------------------------------------------------------------------
	void GameNetworkServer::PlayerReady(Id clientId)
	{

		m_playersLocker.lock(); // <------------- LOCK
		if (m_playersInfos.find(clientId) == m_playersInfos.end())
		{
			m_playersLocker.unlock(); // <------------- UNLOCK
			return;
		}

		m_playersInfos[clientId].State = DeusPlayerInfos::EPlayerState::Ready;
		DeusCore::Logger::Instance()->Log(m_name, "Client " + std::to_string(clientId) + " is ready !");

		if (CanStartGame())
		{
			DeusCore::PacketSPtr p_startGamePacket = std::shared_ptr<DeusCore::PacketStartGame>(new DeusCore::PacketStartGame());

			// We add all players ids
			for (auto& player : m_playersInfos)
				std::dynamic_pointer_cast<DeusCore::PacketStartGame>(p_startGamePacket)->AddPlayerConnectionId(player.first);

			DeusCore::EventManagerHandler::Instance()->QueueEvent(m_gameId, m_gameId, p_startGamePacket);
			DeusCore::Logger::Instance()->Log(m_name, "Request start game");
		}
		m_playersLocker.unlock(); // <------------- UNLOCK
	}

	//---------------------------------------------------------------------------------
	void GameNetworkServer::PlayerNotReady(Id clientId)
	{
		m_playersLocker.lock(); // <------------- LOCK
		if (m_playersInfos.find(clientId) == m_playersInfos.end())
		{
			m_playersLocker.unlock(); // <------------- UNLOCK
			return;
		}

		m_playersInfos[clientId].State = DeusPlayerInfos::EPlayerState::NotReady;
		m_playersLocker.unlock(); // <------------- UNLOCK
	}

	//---------------------------------------------------------------------------------
	void GameNetworkServer::ObjectChangedCell(std::shared_ptr<PacketObjectChangeCell> p_packetReceived)
	{
		// 1 - Player update subscriptions : 
		// - send ObjectEnter for each GameObjects in new interest cell
		// - send ObjectLeave for each GameObject in cell that arn't in interest area anymore
		UpdateCellSubscription(p_packetReceived->GetClientId(), p_packetReceived->GetLeftCellId(), p_packetReceived->GetEnteredCellId());

		// 2 - Update gameobjectid for the player
		if (p_packetReceived->IsInit())
			UpdateGameObjectIdForPlayer(p_packetReceived->GetClientId(), p_packetReceived->GetGameObject()->GetId());

		// 3 - Send ObjectLeave packet to other player
		ManageLeftCell(p_packetReceived->GetLeftCellId(), p_packetReceived->GetEnteredCellId(), p_packetReceived->GetGameObject()->GetId(), p_packetReceived->GetObjectInLeavingCells());

		// 4 - Send ObjectEnter packet to other player
		ManageEnteredCell(p_packetReceived->GetLeftCellId(), p_packetReceived->GetEnteredCellId(), p_packetReceived->GetGameObject(), p_packetReceived->GetObjectInEnteringCells(), p_packetReceived->IsInit());

	}

	//---------------------------------------------------------------------------------
	bool GameNetworkServer::CanStartGame()
	{
		m_lockClients.lock(); // <----------------- LOCK
		unsigned int nbrClients = m_clientsConnections.size();
		m_lockClients.unlock();// <----------------- UNLOCK

		unsigned int nbrPlayerReady = 0;

		for (const auto& playerIt : m_playersInfos)
		{
			if (playerIt.second.State >= DeusPlayerInfos::EPlayerState::Ready)
				nbrPlayerReady++;
		}

		return nbrClients <= nbrPlayerReady;
	}

	//---------------------------------------------------------------------------------
	bool GameNetworkServer::IsPlayerFollowingObject(Id playerId, Id objectId)
	{
		bool result = false;

		if (m_playersInfos[playerId].ObjectsIdsFollowed.size() > 0)
		{
			for (const auto& playerObjectIt : m_playersInfos[playerId].ObjectsIdsFollowed)
			{
				if (playerObjectIt == objectId)
				{
					result = true;
					break;
				}
			}
		}

		return result;
	}

	bool GameNetworkServer::IsObjectEnteringInnerArea(Id clientId, CellId source, CellId destination)
	{
		// TODO : Interest management !!
		return true;
	}

	bool GameNetworkServer::IsObjectLeavingOutterArea(Id clientId, CellId source, CellId destination)
	{
		// TODO : Interest management !!
		return true;
	}

	void GameNetworkServer::UpdateCellSubscription(Id clientId, CellId cellLeavedId, CellId cellEnteredId)
	{
		// TODO : Interest management !!

		if (clientId > 0)
		{
			if (cellLeavedId > 0)
			{
				m_cellLocker[cellLeavedId - 1].lock(); // <------- LOCK

				auto listenerIdIt = std::find(m_cells[cellLeavedId].begin(), m_cells[cellLeavedId].end(), clientId);
				if (listenerIdIt != m_cells[cellLeavedId].end())
					m_cells[cellLeavedId].erase(listenerIdIt);

				m_cellLocker[cellLeavedId - 1].unlock(); // <------- UNLOCK
			}

			if (cellEnteredId > 0)
			{
				m_cellLocker[cellEnteredId - 1].lock(); // <------- LOCK
				m_cells[cellEnteredId].push_back(clientId);
				m_cellLocker[cellEnteredId - 1].unlock(); // <------- UNLOCK
			}
		}
	}

	void GameNetworkServer::UpdateGameObjectIdForPlayer(Id clientId, Id objectId)
	{
		if (clientId)
		{
			m_playersLocker.lock(); // <----------------- LOCK
			PlayersInfos::iterator playerIt = m_playersInfos.find(clientId);
			if (playerIt != m_playersInfos.end())
			{
				//DeusCore::Logger::Instance()->Log(m_name, "Associate Client :"+std::to_string(clientId)+ " with GO :"+std::to_string(objectId));
				if (playerIt->second.GameObjectId == 0)
					playerIt->second.GameObjectId = objectId;
				else if (playerIt->second.GameObjectId != objectId)
					DeusCore::Logger::Instance()->Log(m_name, "Strange behavior : try to update ObjectId for the player");
			}
			m_playersLocker.unlock(); // <----------------- UNLOCK

		}
	}

	void GameNetworkServer::ManageLeftCell(CellId cellLeftId, CellId cellEnteredId, Id objectId, const std::vector<std::shared_ptr<const GameObject>>& objectInCellsLeft)
	{
		if (cellLeftId > 0)
		{
			m_cellLocker[cellLeftId - 1].lock(); // <------- LOCK
												 // check if there is listener on that cell
			if (m_cells[cellLeftId].size() > 0)
			{
				for (int listenerId : m_cells[cellLeftId])
				{
					m_playersLocker.lock(); // <----------------- LOCK
					// we don't want to notify our player of his/her own actions
					if (objectId == m_playersInfos[listenerId].GameObjectId)
					{
						//////////////////////////////////////////////////////////////
						// If there is object we left, tell it to the player so he/she can delete it!
						if (objectInCellsLeft.size() > 0)
						{
							for (const auto& gameObjectWeLeave : objectInCellsLeft)
							{
								assert(gameObjectWeLeave->GetId() != m_playersInfos[listenerId].GameObjectId);
								ObjectLeft(gameObjectWeLeave->GetId(), listenerId);
							}
						}
						//////////////////////////////////////////////////////////////

						m_playersLocker.unlock(); // <----------------- UNLOCK
						continue;
					}
					// this client is eligible for the ObjectLeave packet
					// if player already know this object (entered once in inner area)
					// and the object is leaving the outter area
					if (IsPlayerFollowingObject(listenerId, objectId)
						&& IsObjectLeavingOutterArea(listenerId, cellLeftId, cellEnteredId))
					{
						ObjectLeft(objectId, listenerId);
					}
					m_playersLocker.unlock(); // <----------------- UNLOCK

				}
			}
			m_cellLocker[cellLeftId - 1].unlock(); // <------- UNLOCK
		}
	}

	void GameNetworkServer::ManageEnteredCell(CellId cellLeftId, CellId cellEnteredId, std::shared_ptr<const GameObject> object, const std::vector<std::shared_ptr<const GameObject>>& objectInCellsEntered, bool notifyAllPlayer)
	{
		if (cellEnteredId > 0)
		{
			m_cellLocker[cellEnteredId - 1].lock(); // <------- LOCK

			if (m_cells[cellEnteredId].size() > 0)
			{
				for (int listenerId : m_cells[cellEnteredId])
				{
					m_playersLocker.lock(); // <----------------- LOCK
					// we don't want to notify our player of his/her own actions
					// exept if this is the init of the player
					bool listenerIsThisGameObject = object->GetId() == m_playersInfos[listenerId].GameObjectId;
					if (listenerIsThisGameObject)
					{
						//////////////////////////////////////////////////////////////
						// If there is object already here, tell it to the player !
						if (objectInCellsEntered.size() > 0)
						{
							for (const auto& gameObjectAlreadyHere : objectInCellsEntered)
							{
								assert(gameObjectAlreadyHere->GetId() != m_playersInfos[listenerId].GameObjectId);
								ObjectEnter(listenerId, gameObjectAlreadyHere);
							}
						}
						//////////////////////////////////////////////////////////////

						if (!notifyAllPlayer)
						{
							m_playersLocker.unlock(); // <----------------- UNLOCK
							continue;
						}
					}

					//////////////////////////////////////////////////////////////
					// this client is eligible for the ObjectEnter packet
					// if player DON'T already know this object
					// and the object is inetering the inner area
					if (!IsPlayerFollowingObject(listenerId, object->GetId())
						&& IsObjectEnteringInnerArea(listenerId, cellLeftId, cellEnteredId))
					{
						ObjectEnter(listenerId, object);
					}
					//////////////////////////////////////////////////////////////
					
					m_playersLocker.unlock(); // <----------------- UNLOCK
				}
			}
			m_cellLocker[cellEnteredId - 1].unlock(); // <------- UNLOCK
		}
	}

	void GameNetworkServer::ObjectEnter(Id clientId, std::shared_ptr<const GameObject> gameObject)
	{
		//update id we follow
		m_playersInfos[clientId].ObjectsIdsFollowed.push_back(gameObject->GetId());

		// Send packet ObjectEnter in UDP
		DeusCore::PacketUPtr p_packetEnteredCell = std::unique_ptr<PacketObjectEnter>(new PacketObjectEnter(gameObject->GetId(), gameObject->GetType(), gameObject->GetId() == m_playersInfos[clientId].GameObjectId));
		SendPacket(std::move(p_packetEnteredCell), clientId, SEND_UDP);
	}

	void GameNetworkServer::ObjectLeft(Id objectId, Id clientId)
	{
		// we don't follow this object anymore !
		std::vector<Id>::iterator objectFollowedIt = std::find(m_playersInfos[clientId].ObjectsIdsFollowed.begin(), m_playersInfos[clientId].ObjectsIdsFollowed.end(), objectId);
		if (objectFollowedIt != m_playersInfos[clientId].ObjectsIdsFollowed.end())
			m_playersInfos[clientId].ObjectsIdsFollowed.erase(objectFollowedIt);

		// Send packet ObjectLeave in UDP
		DeusCore::PacketUPtr p_packetLeaveCell = std::unique_ptr<PacketObjectLeave>(new PacketObjectLeave(objectId));
		SendPacket(std::move(p_packetLeaveCell), clientId, SEND_UDP);
	}

}
