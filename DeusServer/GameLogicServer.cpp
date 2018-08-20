#include "GameLogicServer.h"
#include "GameObjectFactory.h"
#include "PacketObjectChangeCell.h"
#include "PacketCellFirePacket.h"
#include "PositionTimeLineComponent.h"
#include "HealthTimeLineComponent.h"

#include "DeusCore/Logger.h"
#include "DeusCore/Packets.h"
#include "DeusCore/EventManagerHandler.h"

namespace DeusServer
{
	GameLogicServer::GameLogicServer(Id gameId)
	{
		m_gameId = gameId;
		m_name = "Logic Server " + std::to_string(gameId);
		DeusCore::Logger::Instance()->Log(m_name, "New " + m_name);

		DeusCore::DeusEventDeleguate messageInterpretDeleguate = fastdelegate::MakeDelegate(this, &GameLogicServer::InterpretPacket);

		DeusCore::EventManagerHandler::Instance()->AddListener(m_gameId, messageInterpretDeleguate, DeusCore::Packet::EMessageType::StartGame);
		DeusCore::EventManagerHandler::Instance()->AddListener(m_gameId, messageInterpretDeleguate, DeusCore::Packet::EMessageType::Disconnect);
		DeusCore::EventManagerHandler::Instance()->AddListener(m_gameId, messageInterpretDeleguate, DeusCore::Packet::EMessageType::LeaveGameRequest);

		DeusCore::EventManagerHandler::Instance()->AddListener(m_gameId, messageInterpretDeleguate, DeusCore::Packet::EMessageType::UpdateMovementRequest);
	}

	GameLogicServer::~GameLogicServer()
	{
		if (!m_isStopped)
			Stop();
	}

	void GameLogicServer::OnUpdate(double deltatimeMs)
	{
		m_cellLock.lock();
		for (const auto& cellGameObjects : m_cellsGameObjects)
		{
			//m_gameObjLocker[cellGameObjects.first - 1].lock(); // <-----------LOCK
			for (const auto& gameObj : cellGameObjects.second)
			{
				gameObj.second->Update(deltatimeMs);
			}
			//m_gameObjLocker[cellGameObjects.first - 1].unlock(); // <-----------UNLOCK
		}
		m_cellLock.unlock();
	}

	void GameLogicServer::OnStart()
	{
		m_cellLock.lock();
		for (const auto& cellGameObjects : m_cellsGameObjects)
		{
			//m_gameObjLocker[cellGameObjects.first - 1].lock(); // <-----------LOCK
			for (const auto& gameObj : cellGameObjects.second)
			{
				gameObj.second->Start();
			}
			//m_gameObjLocker[cellGameObjects.first - 1].unlock(); // <-----------UNLOCK
		}
		m_cellLock.unlock();
	}

	void GameLogicServer::OnStop()
	{
		DeusCore::DeusEventDeleguate messageInterpretDeleguate = fastdelegate::MakeDelegate(this, &GameLogicServer::InterpretPacket);
		DeusCore::EventManagerHandler::Instance()->RemoveListener(m_gameId, messageInterpretDeleguate, DeusCore::Packet::EMessageType::StartGame);
		DeusCore::EventManagerHandler::Instance()->RemoveListener(m_gameId, messageInterpretDeleguate, DeusCore::Packet::EMessageType::Disconnect);
		DeusCore::EventManagerHandler::Instance()->RemoveListener(m_gameId, messageInterpretDeleguate, DeusCore::Packet::EMessageType::LeaveGameRequest);

		DeusCore::EventManagerHandler::Instance()->RemoveListener(m_gameId, messageInterpretDeleguate, DeusCore::Packet::EMessageType::UpdateMovementRequest);


		m_cellsGameObjects.clear();
		m_playerWithGameObject.clear();

		DeusCore::Logger::Instance()->Log(m_name, "Stopped");
	}

	bool GameLogicServer::AddObject(std::shared_ptr<GameObject> p_objectToAdd, Id cellId)
	{
		m_cellLock.lock();
		//m_gameObjLocker[cellId - 1].lock(); // <-----------LOCK
		if (m_cellsGameObjects[cellId].find(p_objectToAdd->GetId()) != m_cellsGameObjects[cellId].end())
		{
			//m_gameObjLocker[cellId - 1].unlock(); // <-----------UNLOCK

			m_cellLock.unlock();
			return false; // Already existing game object !
		}

		p_objectToAdd->Start();
		//DeusCore::Logger::Instance()->Log(m_name, "Create GameObject : " + std::to_string(p_objectToAdd->GetId()));
		m_cellsGameObjects[cellId].insert(std::make_pair(p_objectToAdd->GetId(), p_objectToAdd));

		//m_gameObjLocker[cellId - 1].unlock(); // <-----------UNLOCK

		m_cellLock.unlock();
		return true;
	}

	bool GameLogicServer::RemoveObject(uint32_t objectToDeleteId)
	{
		m_cellLock.lock();
		for (auto& cellGameObjects : m_cellsGameObjects)
		{
			//m_gameObjLocker[cellGameObjects.first - 1].lock(); // <-----------LOCK

			const auto& gameObjIt = cellGameObjects.second.find(objectToDeleteId);
			if (gameObjIt != cellGameObjects.second.end())
			{
				gameObjIt->second->Stop();
				cellGameObjects.second.erase(gameObjIt);
				//m_gameObjLocker[cellGameObjects.first - 1].unlock(); // <-----------UNLOCK

				m_cellLock.unlock();
				return true;
			}
			//m_gameObjLocker[cellGameObjects.first - 1].unlock(); // <-----------UNLOCK
		}

		m_cellLock.unlock();
		return false;
	}

	void GameLogicServer::InterpretPacket(DeusCore::DeusEventSPtr p_packet)
	{
		switch (p_packet->second->GetType())
		{
		case DeusCore::Packet::EMessageType::StartGame:
			StartNewGame(((DeusCore::PacketStartGame*)p_packet->second.get())->GetPlayerConnectionId());
			break;
		case DeusCore::Packet::EMessageType::Disconnect:
			PlayerDisconnected(p_packet->first);
			break;
		case DeusCore::Packet::EMessageType::LeaveGameRequest:
			PlayerDisconnected(p_packet->first);
			break;
		case DeusCore::Packet::EMessageType::UpdateMovementRequest:
			UpdatePlayerDirection(p_packet->first, std::dynamic_pointer_cast<DeusCore::PacketUpdateMovementRequest>(p_packet->second)->GetComponentId(), std::dynamic_pointer_cast<DeusCore::PacketUpdateMovementRequest>(p_packet->second)->GetNewPosition());
			break;
		}
	}

	void GameLogicServer::StartNewGame(const std::vector<uint32_t>& playerIds)
	{
		//DeusCore::Logger::Instance()->Log(m_name, "Yo");
		// 1 - Init cells
		for (size_t i = 0; i < NUMBER_CELLS; i++)
		{
			// We start Cells id at 1
			m_cellsGameObjects.insert(std::make_pair(i + 1, ServerGameObjects()));
		}

		// 2 - Create a GameObject for each player
		for (const auto id : playerIds)
		{
			if (m_playerWithGameObject.find(id) != m_playerWithGameObject.end())
				continue;// already init this player

			std::shared_ptr<GameObject> p_gameObj = GameObjectFactory::Create(GameObject::EObjectType::Player);
			m_playerWithGameObject[id] = p_gameObj->GetId();
			AddObject(p_gameObj, DEFAULT_CELL_ID); // give ownership

			// TODO : Manage this in futur PositionComponent -> manage enter/leave cell
			// Get already existing data
			std::vector<std::shared_ptr<const GameObject>> objectInCellsLeft;
			std::vector<std::shared_ptr<const GameObject>> objectInCellsEntered;
			GetGameObjectOnChangeCells(id, 0, DEFAULT_CELL_ID, objectInCellsLeft, objectInCellsEntered);


			DeusCore::PacketSPtr p_packet = std::shared_ptr<PacketObjectChangeCell>(new PacketObjectChangeCell(id, p_gameObj, 0, DEFAULT_CELL_ID, objectInCellsLeft, objectInCellsEntered, true));
			DeusCore::EventManagerHandler::Instance()->QueueEvent(m_gameId, 0, p_packet);
		}

		for (const auto pGo : m_playerWithGameObject)
			DeusCore::Logger::Instance()->Log(m_name, "Player : " + std::to_string(pGo.first) + " | GameObj : " + std::to_string(pGo.second));

		// 3 - Start the game logic
		Start();


	}

	void GameLogicServer::PlayerDisconnected(Id clientId)
	{
		Id objectId = 0;

		m_playersLocker.lock(); // <---------- LOCK
		// Delete player disconnected
		PlayerInfos::iterator playerIt = m_playerWithGameObject.find(clientId);
		if (playerIt != m_playerWithGameObject.end())
		{
			objectId = playerIt->second;
			m_playerWithGameObject.erase(playerIt);
		}
		m_playersLocker.unlock(); // <---------- UNLOCK

		if (objectId > 0)
		{
			Id cellId = 0; // 0 = error, ids start at 1
			m_cellLock.lock();
			for (auto& cellGameObjects : m_cellsGameObjects)
			{
				//m_gameObjLocker[cellGameObjects.first - 1].lock(); // <-----------LOCK

				const auto& gameObjIt = cellGameObjects.second.find(objectId);
				if (gameObjIt != cellGameObjects.second.end())
				{
					//////////////////////////////////////////////
					m_playersLocker.lock(); // <---------- LOCK					
					if (m_playerWithGameObject.size() > 0)
					{
						DeusCore::PacketSPtr p_packet = std::shared_ptr<PacketObjectChangeCell>(new PacketObjectChangeCell(clientId, gameObjIt->second, cellGameObjects.first, (Id)0));
						DeusCore::EventManagerHandler::Instance()->QueueEvent(m_gameId, 0, p_packet);
					}
					m_playersLocker.unlock(); // <---------- UNLOCK

					//GameObject found, delete it and send infos
					cellGameObjects.second.erase(gameObjIt);
					//////////////////////////////////////////////
					//m_gameObjLocker[cellGameObjects.first - 1].unlock(); // <-----------UNLOCK
					break;
				}
				//m_gameObjLocker[cellGameObjects.first - 1].unlock(); // <-----------UNLOCK
			}
			m_cellLock.unlock();
		}
	}

	void GameLogicServer::UpdatePlayerDirection(Id clientId, Id componentId, DeusCore::DeusVector2 destination)
	{
		//DeusCore::Logger::Instance()->Log(m_name, "Yo");

		GameObjectId objectId = 0;

		m_playersLocker.lock(); // <----------- LOCK
		// Find object
		if (m_playerWithGameObject.find(clientId) != m_playerWithGameObject.end())
		{
			objectId = m_playerWithGameObject[clientId];
		}
		m_playersLocker.unlock(); // <----------- UNLOCK

		if (objectId > 0)
		{
			try
			{
				m_cellLock.lock();
				for (auto& cellGameObjects : m_cellsGameObjects)
				{
					//m_gameObjLocker[cellGameObjects.first - 1].lock(); // <-----------LOCK

					// Search for object
					const auto& gameObjIt = cellGameObjects.second.find(objectId);
					if (gameObjIt != cellGameObjects.second.end())
					{
						// Search for PositionComponent
						std::shared_ptr<GameObjectComponent> compo = gameObjIt->second->GetComponent(componentId);
						if (compo != nullptr)
						{
							// 1 - Validate current position : position of the object in DELAY ms
							uint32_t currentMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() + DELAY_MS;

							std::shared_ptr<const DeusCore::DeusVector2> p_posAtUpdate = std::dynamic_pointer_cast<PositionTimeLineComponent>(compo)->GetValue(currentMs);
							assert(p_posAtUpdate);
							std::dynamic_pointer_cast<PositionTimeLineComponent>(compo)->InsertData(p_posAtUpdate, currentMs);

							// 2 - Extrapolate time to go to destination and insert futur data
							float sqrtDist = DeusCore::DeusVector2::SqrtMagnitude(*p_posAtUpdate, destination);
							uint32_t dtReachDestinationMs = sqrtDist / SPEED_MS; // t = d / s
							
							DeusCore::Logger::Instance()->Log(m_name, "reach destination in : "+std::to_string(dtReachDestinationMs)+"ms at : "+std::to_string(currentMs + dtReachDestinationMs));

							std::dynamic_pointer_cast<PositionTimeLineComponent>(compo)->InsertData(std::make_shared<const DeusCore::DeusVector2>(destination), currentMs + dtReachDestinationMs);

							// 3 - Send messages to clients
							std::unique_ptr<DeusCore::PacketUpdateMovementAnswer> movementFeedback = std::unique_ptr<DeusCore::PacketUpdateMovementAnswer>(new DeusCore::PacketUpdateMovementAnswer(objectId, componentId, *p_posAtUpdate, currentMs, destination, currentMs + dtReachDestinationMs));
							DeusCore::PacketSPtr p_cellFirePacket = std::shared_ptr<CellFirePacket>(new CellFirePacket(cellGameObjects.first, objectId, std::move(movementFeedback)));
							DeusCore::EventManagerHandler::Instance()->QueueEvent(m_gameId, 0, p_cellFirePacket);

							//DeusCore::Logger::Instance()->Log(m_name, "UpdatePlayerDirection success");
							break;
						}
					}

					//m_gameObjLocker[cellGameObjects.first - 1].unlock(); // <-----------UNLOCK
				}
				m_cellLock.unlock();
			}
			catch (const std::system_error& e)
			{
				DeusCore::Logger::Instance()->Log(m_name, e.what());
			}
		}
	}

	Id GameLogicServer::GetCellIdOfGameObject(Id objectId)
	{
		Id cellId = 0; // 0 = error, ids start at 1
		m_cellLock.lock();
		for (auto& cellGameObjects : m_cellsGameObjects)
		{
			//m_gameObjLocker[cellGameObjects.first - 1].lock(); // <-----------LOCK

			const auto& gameObjIt = cellGameObjects.second.find(objectId);
			if (gameObjIt != cellGameObjects.second.end())
			{
				cellId = cellGameObjects.first;
				//m_gameObjLocker[cellGameObjects.first - 1].unlock(); // <-----------UNLOCK
				m_cellLock.unlock(); 
				return cellId;
			}
			//m_gameObjLocker[cellGameObjects.first - 1].unlock(); // <-----------UNLOCK
		}
		m_cellLock.unlock();
		return cellId;
	}

	void GameLogicServer::GetGameObjectOnChangeCells(Id playerId, Id cellLeavedId, Id cellEnteredId, std::vector<std::shared_ptr<const GameObject>>& objectInCellsLeft, std::vector<std::shared_ptr<const GameObject>>& objectInCellsEntered)
	{
		// TODO : Interest management here !
		// For now, we juste assert that there is 1 cell

		if (cellEnteredId > 0 && cellEnteredId < NUMBER_CELLS + 1)
		{
			m_cellLock.lock();
			for (const auto& gameObj : m_cellsGameObjects[cellEnteredId])
			{
				if (gameObj.second->GetId() != m_playerWithGameObject[playerId])
					objectInCellsEntered.push_back(gameObj.second);
			}m_cellLock.unlock();
		}
	}
}
