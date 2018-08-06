#include "GameLogicServer.h"
#include "GameObjectFactory.h"
#include "PacketObjectEnter.h"

#include "DeusCore/Logger.h"
#include "DeusCore/Packet.h"
#include "DeusCore/EventManagerHandler.h"

namespace DeusServer
{
	GameLogicServer::GameLogicServer(int gameId)
	{
		m_gameId = gameId;
		m_name = "Logic Server " + std::to_string(gameId);
		DeusCore::Logger::Instance()->Log(m_name, "New " + m_name);
	}

	GameLogicServer::~GameLogicServer()
	{
		if (!m_isStopped)
			Stop();
	}

	void GameLogicServer::OnUpdate(double deltatimeMs)
	{
		for (const auto& gameObj : m_gameObjects) {
			gameObj.second->Update(deltatimeMs);
		}
	}

	void GameLogicServer::OnStart()
	{

	}

	void GameLogicServer::OnStop()
	{

	}

	bool GameLogicServer::AddObject(std::shared_ptr<GameObject> p_objectToAdd)
	{
		m_gameObjLocker.lock(); // <-----------LOCK
		if (m_gameObjects.find(p_objectToAdd->GetId()) != m_gameObjects.end())
		{
			m_gameObjLocker.unlock(); // <-----------UNLOCK
			return false; // Already existing game object !
		}

		p_objectToAdd->Start();
		m_gameObjects.insert(std::make_pair(p_objectToAdd->GetId(), p_objectToAdd));

		m_gameObjLocker.unlock(); // <-----------UNLOCK
		return true;
	}
	bool GameLogicServer::RemoveObject(uint32_t objectToDeleteId)
	{
		m_gameObjLocker.lock(); // <-----------LOCK
		const auto& gameObjIt = m_gameObjects.find(objectToDeleteId);
		if (gameObjIt == m_gameObjects.end())
		{
			m_gameObjLocker.unlock(); // <-----------UNLOCK
			return false; // don't found matching ids
		}

		gameObjIt->second->Stop();
		m_gameObjects.erase(gameObjIt);

		m_gameObjLocker.unlock(); // <-----------UNLOCK
		return true;
	}

	void GameLogicServer::InterpretPacket(DeusCore::DeusEventSPtr p_packet)
	{
		switch (p_packet->second->GetType())
		{
		case DeusCore::Packet::EMessageType::StartGame:

			StartNewGame(((DeusCore::PacketStartGame*)p_packet->second.get())->GetPlayerConnectionId());
			break;
		}
	}

	void GameLogicServer::StartNewGame(const std::vector<uint32_t>& playerIds)
	{
		// Create a PlayerGameObject for each player
		for (const auto id : playerIds)
		{
			if (m_playerWithGameObject.find(id) != m_playerWithGameObject.end())
				continue;// already init this player

			std::unique_ptr<GameObject> p_gameObj = GameObjectFactory::Create(GameObject::EObjectType::Player);
			m_playerWithGameObject[id] = p_gameObj->GetId();
			AddObject(std::move(p_gameObj)); // give ownership
		}

		Start();

		// TODO : Manage this in futur PositionComponent -> manage enter/leave cell
		for (const auto& playerWithGO : m_playerWithGameObject)
		{
			DeusCore::PacketSPtr p_packet = std::shared_ptr<DeusServer::PacketObjectEnter>(new DeusServer::PacketObjectEnter((uint32_t)playerWithGO.second, GameObject::EObjectType::Player));
			DeusCore::EventManagerHandler::Instance()->QueueEvent(m_gameId, 0, p_packet);
		}
	}
}
