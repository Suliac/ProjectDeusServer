#include "WorldNetworkServer.h"

#include "DeusCore\DeusSocketException.h"
#include "DeusCore\EventManagerHandler.h"
#include "DeusCore\Logger.h"

#include "DeusCore\Packets.h"
#include "PacketNewPlayer.h"
#include <stdio.h>

namespace DeusServer
{
	WorldNetworkServer::WorldNetworkServer()
	{
		m_name = "World Server";
		m_nextGameId = 1;
	}

	WorldNetworkServer::~WorldNetworkServer()
	{
		if (!m_stopped)
		{
			OnStop();
		}
	}

	/////////////////////////// 
	//         SERVER        // 
	///////////////////////////

	//---------------------------------------------------------------------------------
	void WorldNetworkServer::OnStart()
	{
		// 1 - subscribe to our EventQueue
		InitDeleguate();

		//  2 - Launch the thread which handle inputs
		m_inputHandlerThread = std::thread([this] { HandleInput(); });

		//  3 - Init listener
		DeusCore::Logger::Instance()->Log(m_name, "Init & Start tcp listener");
		m_listener.Init(m_ipAddr, m_port, true);
		m_listener.Start();

		// 4 - launch thread to accept new connections
		m_acceptConnectionsThread = std::thread([this] { AcceptConnection(); });
	}
	
	//---------------------------------------------------------------------------------
	void WorldNetworkServer::OnStop()
	{
		// 1 - Unsub from EventQueue
		DeleteDeleguate();

		// 2 - We stop the listener socket
		m_listener.SocketShutdown();
		m_listener.SocketClose();

		if (m_acceptConnectionsThread.joinable())
			m_acceptConnectionsThread.join();

		// 4 - we wait for the end of the input handler thread
		if (m_inputHandlerThread.joinable())
			m_inputHandlerThread.join();

		// 5 - clear games
		m_gamesLocker.lock();		// <--------------- LOCK
		for (DeusGames::iterator it = m_games.begin(); it != m_games.end(); it++)
			it->second->Stop();
		m_gamesLocker.unlock();		// <--------------- UNLOCK

		m_gamesLocker.lock();		// <--------------- LOCK
		m_games.clear();
		m_gamesLocker.unlock();		// <--------------- UNLOCK

		m_stopped = true;
	}

	//---------------------------------------------------------------------------------
	void WorldNetworkServer::OnDisconnectClient(int clientId)
	{
	}

	//---------------------------------------------------------------------------------
	// Threaded function
	//---------------------------------------------------------------------------------
	void WorldNetworkServer::AcceptConnection()
	{
		m_nextClientId = 1;
		while (!m_wantToStop)
		{
			// create a pointer on empty tcp socket
			std::unique_ptr<DeusCore::TcpSocket> p_client = std::make_unique<DeusCore::TcpSocket>();

			// timeout is in the accept function
			// block until timeout ('timeoutSecond' sec currently)
			if (m_listener.Accept(*p_client, DEFAULT_SOCKETSTATE_TIMEOUT))
			{
				DeusCore::Logger::Instance()->Log(m_name, "Connection accepted !");

				// we want to transfert the ownership of our TCP socket to our DeusClient instance
				// which handle the communication behavior for TCP (and UDP too in the futur)
				m_connectionsLocker.lock(); // <------------------- LOCK
				m_clientsConnections.insert(
					std::make_pair(
						m_nextClientId,
						DeusClientSPtr(new DeusServer::DeusClient(m_nextClientId, std::move(p_client)))));
				m_connectionsLocker.unlock(); // <------------------- UNLOCK

				m_nextClientId++;
			}
		}

	}

	//---------------------------------------------------------------------------------
	// Threaded function
	//---------------------------------------------------------------------------------
	void WorldNetworkServer::HandleInput()
	{
		while (!m_wantToStop)
		{
			// Get keyboard input
			std::string cmd;
			std::getline(std::cin, cmd);

			if (cmd == "stop")
			{
				DeusCore::Logger::Instance()->Log(m_name, "Stop requested");
				m_wantToStop = true;
			}
			else {
				DeusCore::Logger::Instance()->Log(m_name, "Command unkown : to stop the server enter 'stop'");
			}
		}

		DeusCore::Logger::Instance()->Log(m_name, "End HandleInput Thread");
	}


	/////////////////////////// 
	//       DELEGUATES      // 
	///////////////////////////

	//---------------------------------------------------------------------------------
	void WorldNetworkServer::InitDeleguate()
	{
		// create deleguate functions
		DeusCore::DeusEventDeleguate messageInterpretDeleguate = fastdelegate::MakeDelegate(this, &WorldNetworkServer::InterpretPacket);

		// register deleguates with connection
		DeusCore::EventManagerHandler::Instance()->AddListener(0, messageInterpretDeleguate, DeusCore::Packet::EMessageType::Test);
		DeusCore::EventManagerHandler::Instance()->AddListener(0, messageInterpretDeleguate, DeusCore::Packet::EMessageType::CreateGameRequest);
		DeusCore::EventManagerHandler::Instance()->AddListener(0, messageInterpretDeleguate, DeusCore::Packet::EMessageType::JoinGameRequest);
		DeusCore::EventManagerHandler::Instance()->AddListener(0, messageInterpretDeleguate, DeusCore::Packet::EMessageType::GetGameRequest);
		DeusCore::EventManagerHandler::Instance()->AddListener(0, messageInterpretDeleguate, DeusCore::Packet::EMessageType::DeleteGameRequest);

		//NB : Disconnect event already managed in NetworkServer
	}

	void WorldNetworkServer::DeleteDeleguate()
	{
		// create deleguate functions
		DeusCore::DeusEventDeleguate messageInterpretDeleguate = fastdelegate::MakeDelegate(this, &WorldNetworkServer::InterpretPacket);

		// delete deleguates with connection
		DeusCore::EventManagerHandler::Instance()->RemoveListener(0, messageInterpretDeleguate, DeusCore::Packet::EMessageType::Test);
		DeusCore::EventManagerHandler::Instance()->RemoveListener(0, messageInterpretDeleguate, DeusCore::Packet::EMessageType::CreateGameRequest);
		DeusCore::EventManagerHandler::Instance()->RemoveListener(0, messageInterpretDeleguate, DeusCore::Packet::EMessageType::JoinGameRequest);
		DeusCore::EventManagerHandler::Instance()->RemoveListener(0, messageInterpretDeleguate, DeusCore::Packet::EMessageType::GetGameRequest);
		DeusCore::EventManagerHandler::Instance()->RemoveListener(0, messageInterpretDeleguate, DeusCore::Packet::EMessageType::DeleteGameRequest);

		//NB : Disconnect event already managed in NetworkServer
	}

	/////////////////////////// 
	//          GAMES        // 
	///////////////////////////

	//---------------------------------------------------------------------------------
	void WorldNetworkServer::InterpretPacket(DeusCore::DeusEventSPtr p_packet)
	{
		// Nb : call from EventManager thread during the process of events
		switch (p_packet->second->GetID())
		{
		case DeusCore::Packet::EMessageType::Error:
			DeusCore::Logger::Instance()->Log(m_name, "Try to interpret packet failed. Unknown packet id : " + std::to_string(p_packet->second->GetID()) + ". Sent by client :" + std::to_string(p_packet->first));
			break;

		case DeusCore::Packet::EMessageType::Test:
			//DeusCore::Logger::Instance()->Log(m_name, "Interpret packet id : " + std::to_string(p_packet->second->GetID()) + ". Sent by client :" + std::to_string(p_packet->first));
			DeusCore::Logger::Instance()->Log(m_name, "Message : " + ((DeusCore::PacketTest*)p_packet->second.get())->GetTextMessage());
			break;

		case DeusCore::Packet::EMessageType::CreateGameRequest:
			CreateGame(p_packet->first);
			break;

		case DeusCore::Packet::EMessageType::JoinGameRequest:
			JoinGame(p_packet->first, ((DeusCore::PacketJoinGameRequest*)p_packet->second.get())->GetGameId());
			break;

		case DeusCore::Packet::EMessageType::GetGameRequest:
			GetGames(p_packet->first);
			break;

		case DeusCore::Packet::EMessageType::DeleteGameRequest:
			DeleteGame(p_packet->first);
			break;
			//NB : Disconnect event already managed in NetworkServer
		default:
			break;
		}
	}

	//---------------------------------------------------------------------------------
	void WorldNetworkServer::GetGames(int clientId)
	{
		std::vector<unsigned int> gamesIds;
		m_gamesLocker.lock(); // <------------------ LOCK
		for (DeusGames::iterator it = m_games.begin(); it != m_games.end(); ++it) {
			gamesIds.push_back(it->first);
		}
		m_gamesLocker.unlock();// <------------------ UNLOCK

		// Send answer
		std::unique_ptr<DeusCore::PacketGetGamesAnswer> p_packet = std::unique_ptr<DeusCore::PacketGetGamesAnswer>(new DeusCore::PacketGetGamesAnswer());
		p_packet->SetGames(gamesIds);
		p_packet->SetSuccess(true);
		SendPacket(std::move(p_packet), clientId, true);

		DeusCore::Logger::Instance()->Log(m_name, "Client (id:" + std::to_string(clientId) + ") wants to get games");
	}

	//---------------------------------------------------------------------------------
	void WorldNetworkServer::CreateGame(int clientId)
	{
		m_connectionsLocker.lock(); // <-------------- LOCK

		DeusClientConnections::iterator findClientIt = m_clientsConnections.find(clientId);
		if (findClientIt != m_clientsConnections.end())
		{
			m_gamesLocker.lock(); // <------------------ LOCK
			m_games.insert(std::make_pair(m_nextGameId, std::unique_ptr<GameHandler>(new GameHandler(m_nextGameId))));
			m_gamesLocker.unlock();// <------------------ UNLOCK

			// we queue a server event for the game 
			std::shared_ptr<DeusCore::Packet> p_packet = std::shared_ptr<DeusServer::PacketNewPlayer>(new DeusServer::PacketNewPlayer(findClientIt->second));
			DeusCore::EventManagerHandler::Instance()->QueueEvent(m_nextGameId, clientId, p_packet);

			m_nextGameId++;
		}

		m_connectionsLocker.unlock(); // <-------------- UNLOCK
		DeusCore::Logger::Instance()->Log(m_name, "Client (id:" + std::to_string(clientId) + ") wants to create game");
	}

	//---------------------------------------------------------------------------------
	void WorldNetworkServer::JoinGame(int clientId, int gameId)
	{
		m_connectionsLocker.lock(); // <-------------- LOCK

		DeusClientConnections::iterator findClientIt = m_clientsConnections.find(clientId);
		if (findClientIt != m_clientsConnections.end())
		{
			std::shared_ptr<DeusCore::Packet> p_packet = std::shared_ptr<DeusServer::PacketNewPlayer>(new DeusServer::PacketNewPlayer(findClientIt->second));
			DeusCore::EventManagerHandler::Instance()->QueueEvent(gameId, clientId, p_packet);
		}

		m_connectionsLocker.unlock(); // <-------------- UNLOCK
		DeusCore::Logger::Instance()->Log(m_name, "Client (id:" + std::to_string(clientId) + ") wants to join game");
	}

	void WorldNetworkServer::DeleteGame(int gameId)
	{
		m_gamesLocker.lock(); // <------------------ LOCK
		DeusGames::iterator findGameIt = m_games.find(gameId);
		if (findGameIt != m_games.end())
		{
			findGameIt->second->Stop();
			m_games.erase(findGameIt);
			DeusCore::Logger::Instance()->Log(m_name, "Delete game number : " + std::to_string(gameId));

		}
		m_gamesLocker.unlock();// <------------------ UNLOCK
		DeusCore::Logger::Instance()->Log(m_name, "Game " + std::to_string(gameId) + " will be removed");
	}

}
