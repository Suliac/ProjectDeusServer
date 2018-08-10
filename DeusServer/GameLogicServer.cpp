#include "GameLogicServer.h"
#include "GameObjectFactory.h"
#include "PacketObjectChangeCell.h"

#include "DeusCore/Logger.h"
#include "DeusCore/Packet.h"
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
	}

	GameLogicServer::~GameLogicServer()
	{
		if (!m_isStopped)
			Stop();
	}

	void GameLogicServer::OnUpdate(double deltatimeMs)
	{
		for (const auto& cellGameObjects : m_cellsGameObjects)
		{
			m_gameObjLocker[cellGameObjects.first - 1].lock(); // <-----------LOCK
			for (const auto& gameObj : cellGameObjects.second)
			{
				gameObj.second->Update(deltatimeMs);
			}
			m_gameObjLocker[cellGameObjects.first - 1].unlock(); // <-----------UNLOCK
		}
	}

	void GameLogicServer::OnStart()
	{

	}

	void GameLogicServer::OnStop()
	{
		DeusCore::DeusEventDeleguate messageInterpretDeleguate = fastdelegate::MakeDelegate(this, &GameLogicServer::InterpretPacket);
		DeusCore::EventManagerHandler::Instance()->RemoveListener(m_gameId, messageInterpretDeleguate, DeusCore::Packet::EMessageType::StartGame);
		DeusCore::EventManagerHandler::Instance()->RemoveListener(m_gameId, messageInterpretDeleguate, DeusCore::Packet::EMessageType::Disconnect);

		m_cellsGameObjects.clear();
		m_playerWithGameObject.clear();

		DeusCore::Logger::Instance()->Log(m_name, "Stopped");
	}

	bool GameLogicServer::AddObject(std::shared_ptr<GameObject> p_objectToAdd, Id cellId)
	{
		m_gameObjLocker[cellId - 1].lock(); // <-----------LOCK
		if (m_cellsGameObjects[cellId].find(p_objectToAdd->GetId()) != m_cellsGameObjects[cellId].end())
		{
			m_gameObjLocker[cellId - 1].unlock(); // <-----------UNLOCK
			return false; // Already existing game object !
		}

		p_objectToAdd->Start();
		//DeusCore::Logger::Instance()->Log(m_name, "Create GameObject : " + std::to_string(p_objectToAdd->GetId()));
		m_cellsGameObjects[cellId].insert(std::make_pair(p_objectToAdd->GetId(), p_objectToAdd));

		m_gameObjLocker[cellId - 1].unlock(); // <-----------UNLOCK
		return true;
	}

	bool GameLogicServer::RemoveObject(uint32_t objectToDeleteId)
	{
		for (auto& cellGameObjects : m_cellsGameObjects)
		{
			m_gameObjLocker[cellGameObjects.first - 1].lock(); // <-----------LOCK

			const auto& gameObjIt = cellGameObjects.second.find(objectToDeleteId);
			if (gameObjIt != cellGameObjects.second.end())
			{
				gameObjIt->second->Stop();
				cellGameObjects.second.erase(gameObjIt);
				m_gameObjLocker[cellGameObjects.first - 1].unlock(); // <-----------UNLOCK
				return true;
			}
			m_gameObjLocker[cellGameObjects.first - 1].unlock(); // <-----------UNLOCK
		}

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
			for (auto& cellGameObjects : m_cellsGameObjects)
			{
				m_gameObjLocker[cellGameObjects.first - 1].lock(); // <-----------LOCK

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
					m_gameObjLocker[cellGameObjects.first - 1].unlock(); // <-----------UNLOCK
					break;
				}
				m_gameObjLocker[cellGameObjects.first - 1].unlock(); // <-----------UNLOCK
			}
		}
	}

	Id GameLogicServer::GetCellIdOfGameObject(Id objectId)
	{
		Id cellId = 0; // 0 = error, ids start at 1
		for (auto& cellGameObjects : m_cellsGameObjects)
		{
			m_gameObjLocker[cellGameObjects.first - 1].lock(); // <-----------LOCK

			const auto& gameObjIt = cellGameObjects.second.find(objectId);
			if (gameObjIt != cellGameObjects.second.end())
			{
				cellId = cellGameObjects.first;
				m_gameObjLocker[cellGameObjects.first - 1].unlock(); // <-----------UNLOCK
				return cellId;
			}
			m_gameObjLocker[cellGameObjects.first - 1].unlock(); // <-----------UNLOCK
		}

		return cellId;
	}

	void GameLogicServer::GetGameObjectOnChangeCells(Id playerId, Id cellLeavedId, Id cellEnteredId, std::vector<std::shared_ptr<const GameObject>>& objectInCellsLeft, std::vector<std::shared_ptr<const GameObject>>& objectInCellsEntered)
	{
		// TODO : Interest management here !
		// For now, we juste assert that there is 1 cell

		if (cellEnteredId > 0 && cellEnteredId < NUMBER_CELLS + 1)
		{
			for (const auto& gameObj : m_cellsGameObjects[cellEnteredId])
			{
				if (gameObj.second->GetId() != m_playerWithGameObject[playerId])
					objectInCellsEntered.push_back(gameObj.second);
			}
		}
	}
}
