#include "GameNetworkServer.h"

#include "DeusCore/Logger.h"
#include "DeusCore/Packets.h"
#include "PacketNewPlayer.h"

namespace DeusServer
{
	GameNetworkServer::GameNetworkServer(int gameId)
	{
		m_gameId = gameId;
		m_name = "Game Server " + std::to_string(gameId);
		DeusCore::Logger::Instance()->Log(m_name, "New " + m_name);
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

		m_stopped = true;
	}

	//---------------------------------------------------------------------------------
	void GameNetworkServer::OnDisconnectClient(int clientId)
	{
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

			//NB : Disconnect event already managed in NetworkServer
		default:
			break;
		}
	}

	//---------------------------------------------------------------------------------
	void GameNetworkServer::LeaveGame(unsigned int clientId)
	{
		m_lockClients.lock(); // <----------------- LOCK
		DeusClientConnections::iterator matchingConnectionIt = m_clientsConnections.find(clientId);
		if (matchingConnectionIt != m_clientsConnections.end())
		{
			matchingConnectionIt->second->SetConnectionGameId(0);
			m_clientsConnections.erase(matchingConnectionIt);
			//DeusCore::Logger::Instance()->Log(m_name, "Client " + std::to_string(clientId) + " successfully leaved the game. There are now : " + std::to_string(m_clientsConnections.size()) + "client(s) connected");

			// we check if there is player left, otherwise we stop this game server
			TryDeleteGame();
			DeusCore::Logger::Instance()->Log(m_name, "Client (id:" + std::to_string(clientId) + ") Leaved the game");
		}
		m_lockClients.unlock();// <----------------- UNLOCK
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

	void GameNetworkServer::PlayerReady(unsigned int clientId)
	{
		if (std::find(m_playersReady.begin(), m_playersReady.end(), clientId) != m_playersReady.end())
			return; // player already ready

		m_playersReady.push_back(clientId);
		if (CanStartGame())
		{
			DeusCore::PacketSPtr p_startGamePacket = std::shared_ptr<DeusCore::PacketStartGame>(new DeusCore::PacketStartGame());;
			DeusCore::EventManagerHandler::Instance()->QueueEvent(0, m_gameId, p_startGamePacket);
			DeusCore::Logger::Instance()->Log(m_name, "Request start game");
		}
	}

	void GameNetworkServer::PlayerNotReady(unsigned int clientId)
	{
		const auto& idIt = std::find(m_playersReady.begin(), m_playersReady.end(), clientId);
		if (idIt == m_playersReady.end())
			return; // this player wasn't ready

		m_playersReady.erase(idIt);
	}

	bool GameNetworkServer::CanStartGame()
	{
		m_lockClients.lock(); // <----------------- LOCK
		unsigned int nbrClients = m_clientsConnections.size();
		m_lockClients.unlock();// <----------------- UNLOCK

		return nbrClients <= m_playersReady.size();
	}

	//---------------------------------------------------------------------------------
	bool GameNetworkServer::NewPlayer(int clientId, DeusClientSPtr clientConnection)
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
		
		m_lockClients.unlock();
		return true;
	}

}
