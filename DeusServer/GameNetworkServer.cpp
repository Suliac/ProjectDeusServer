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
		// Unsubscribe from cell
		Id leftCell = 0;
		for (const auto& cell : m_cells)
		{
			m_cellLocker[cell.first - 1].lock(); // <------- LOCK
			auto listenerIdIt = std::find(cell.second.begin(), cell.second.end(), clientId);
			if (listenerIdIt != cell.second.end())
			{
				leftCell = cell.first;
				m_cellLocker[cell.first - 1].unlock(); // <------- UNLOCK
				break;
			}
			m_cellLocker[cell.first - 1].unlock(); // <------- UNLOCK
		}

		UpdateCellSubscription(clientId, leftCell, 0);

		// delete player infos
		PlayersInfos::iterator playerIt = m_playersInfos.find(clientId);
		if (playerIt != m_playersInfos.end())
			m_playersInfos.erase(playerIt);

		// We echo back to the world server the disconnected client id
		DeusCore::PacketSPtr p_disconnectedEvent = std::shared_ptr<DeusCore::PacketClientDisconnect>(new DeusCore::PacketClientDisconnect());
		DeusCore::EventManagerHandler::Instance()->QueueEvent(m_gameId, clientId, p_disconnectedEvent);


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

			// delete player infos
			PlayersInfos::iterator playerIt = m_playersInfos.find(clientId);
			if (playerIt != m_playersInfos.end())
				m_playersInfos.erase(playerIt);

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
		m_playersInfos[clientId] = DeusPlayerInfos();

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
		if (m_playersInfos.find(clientId) == m_playersInfos.end())
			return;

		m_playersInfos[clientId].State = DeusPlayerInfos::EPlayerState::Ready;

		if (CanStartGame())
		{
			DeusCore::PacketSPtr p_startGamePacket = std::shared_ptr<DeusCore::PacketStartGame>(new DeusCore::PacketStartGame());

			// We add all players ids
			for (auto& player : m_playersInfos)
				std::dynamic_pointer_cast<DeusCore::PacketStartGame>(p_startGamePacket)->AddPlayerConnectionId(player.first);

			DeusCore::EventManagerHandler::Instance()->QueueEvent(0, m_gameId, p_startGamePacket);
			DeusCore::Logger::Instance()->Log(m_name, "Request start game");
		}
	}

	//---------------------------------------------------------------------------------
	void GameNetworkServer::PlayerNotReady(Id clientId)
	{
		if (m_playersInfos.find(clientId) == m_playersInfos.end())
			return;

		m_playersInfos[clientId].State = DeusPlayerInfos::EPlayerState::NotReady;
	}

	//---------------------------------------------------------------------------------
	void GameNetworkServer::ObjectChangedCell(std::shared_ptr<PacketObjectChangeCell> p_packetReceived)
	{
		// 1 - Player update subscriptions : 
		// - send ObjectEnter for each GameObjects in new interest cell
		// - send ObjectLeave for each GameObject in cell that arn't in interest area anymore
		if (p_packetReceived->GetClientId() > 0)
			UpdateCellSubscription(p_packetReceived->GetClientId(), p_packetReceived->GetLeftCellId(), p_packetReceived->GetEnteredCellId());

		// 2 - Send ObjectLeave packet to other player
		if (p_packetReceived->GetLeftCellId() > 0)
		{
			m_cellLocker[p_packetReceived->GetLeftCellId() - 1].lock(); // <------- LOCK
			// check if there is listener on that cell
			if (m_cells[p_packetReceived->GetLeftCellId()].size() > 0)
			{
				for (int listenerId : m_cells[p_packetReceived->GetLeftCellId()])
				{
					// we don't want to notify our player of his/her own actions
					if (p_packetReceived->GetObjectId() == m_playersInfos[listenerId].GameObjectId)
						break;


					// this client is eligible for the ObjectLeave packet
					// if player already know this object (entered once in inner area)
					// and the object is leaving the outter area
					if (IsPlayerFollowingObject(listenerId, p_packetReceived->GetObjectId())
						&& IsObjectLeavingOutterArea(listenerId, p_packetReceived->GetLeftCellId(), p_packetReceived->GetEnteredCellId()))
					{
						// Send packet ObjectLeave in UDP
						DeusCore::PacketUPtr p_packetLeaveCell = std::unique_ptr<PacketObjectLeave>(new PacketObjectLeave(p_packetReceived->GetObjectId()));
						SendPacket(std::move(p_packetLeaveCell), listenerId, false);
					}
				}
			}
			m_cellLocker[p_packetReceived->GetLeftCellId() - 1].unlock(); // <------- UNLOCK

		}

		// 3 - Send ObjectEnter packet to other player
		if (p_packetReceived->GetEnteredCellId() > 0)
		{
			m_cellLocker[p_packetReceived->GetEnteredCellId() - 1].lock(); // <------- LOCK

			if (m_cells[p_packetReceived->GetEnteredCellId()].size() > 0)
			{
				for (int listenerId : m_cells[p_packetReceived->GetEnteredCellId()])
				{
					// we don't want to notify our player of his/her own actions
					if (p_packetReceived->GetObjectId() == m_playersInfos[listenerId].GameObjectId)
						break;

					// this client is eligible for the ObjectEnter packet
					// if player DON'T already know this object
					// and the object is inetering the inner area
					if (!IsPlayerFollowingObject(listenerId, p_packetReceived->GetObjectId())
						&& IsObjectEnteringInnerArea(listenerId, p_packetReceived->GetLeftCellId(), p_packetReceived->GetEnteredCellId()))
					{
						// Send packet ObjectLeave in UDP
						DeusCore::PacketUPtr p_packetLeaveCell = std::unique_ptr<PacketObjectLeave>(new PacketObjectLeave(p_packetReceived->GetObjectId()));
						SendPacket(std::move(p_packetLeaveCell), listenerId, false);
					}
				}
			}
			m_cellLocker[p_packetReceived->GetEnteredCellId() - 1].unlock(); // <------- UNLOCK
		}
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

	void GameNetworkServer::UpdateCellSubscription(Id clientId, Id cellLeavedId, Id cellEnteredId)
	{
		// TODO : Interest management !!

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
