#include "WorldNetworkServer.h"

#include "DeusCore\DeusSocketException.h"
#include "DeusCore\Logger.h"

#include "DeusCore\PacketTest.h"
#include "DeusCore\PacketJoinGameRequest.h"
#include "DeusCore\PacketJoinGameAnswer.h"
#include "DeusCore\PacketCreateGameAnswer.h"
#include "DeusCore\PacketGetGamesAnswer.h"

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
		//StopConnections();
		m_listener.SocketShutdown();
		m_listener.SocketClose();
	}

	/////////////////////////// 
	//         SERVER        // 
	///////////////////////////

	void WorldNetworkServer::OnStart()
	{
		// launch the thread which handle inputs
		m_inputHandlerThread = std::thread([this] { HandleInput(); });

		//Init listener
		DeusCore::Logger::Instance()->Log(m_name, "Init & Start tcp listener");
		m_listener.Init(m_ipAddr, m_port, true);
		m_listener.Start();
	}

	void WorldNetworkServer::OnUpdate()
	{
		if (AcceptConnection(m_nextClientId))
			m_nextClientId++;
	}

	void WorldNetworkServer::OnEnd()
	{
		// We stop the listener socket
		m_listener.SocketShutdown();
		m_listener.SocketClose();

		// we wait for the end of the input handler thread
		if (m_inputHandlerThread.joinable())
			m_inputHandlerThread.join();
	}

	void WorldNetworkServer::OnInterpretPacket(int senderId, DeusCore::PacketSPtr p_packet)
	{
		switch (p_packet->GetID())
		{
		case DeusCore::Packet::EMessageType::Test:
			DeusCore::Logger::Instance()->Log(m_name, "Interpret packet id : " + std::to_string(p_packet->GetID()) + ". Sent by client :" + std::to_string(senderId));
			DeusCore::Logger::Instance()->Log(m_name, "Message : " + ((DeusCore::PacketTest*)p_packet.get())->GetTextMessage());
			break;

		case DeusCore::Packet::EMessageType::CreateGameRequest:
			CreateGame(senderId);
			break;

		case DeusCore::Packet::EMessageType::JoinGameRequest:
			JoinGame(senderId, ((DeusCore::PacketJoinGameRequest*)p_packet.get())->GetGameId());
			break;

		case DeusCore::Packet::EMessageType::GetGameRequest:
			GetGames(senderId);
			break;
		default:
			DeusCore::Logger::Instance()->Log(m_name, "Try to interpret packet failed. Unknown packet id : " + std::to_string(p_packet->GetID()) + ". Sent by client :" + std::to_string(senderId));
			break;
		}
	}

	bool WorldNetworkServer::AcceptConnection(const unsigned int id, const unsigned int timeoutSecond)
	{
		// create a pointer on empty tcp socket
		std::unique_ptr<DeusCore::TcpSocket> p_client = std::make_unique<DeusCore::TcpSocket>();

		// timeout is in the accept function
		// block until timeout (3sec currently)
		if (m_listener.Accept(*p_client, timeoutSecond))
		{
			DeusCore::Logger::Instance()->Log(m_name, "Connection accepted !");

			// we want to transfert the ownership of our TCP socket to our DeusClient instance
			// which handle the communication behavior for TCP (and UDP too in the futur)
			m_clientsConnections.insert(
				std::make_pair(
					id,
					DeusClientSPtr(new DeusServer::DeusClient(id, std::move(p_client)))));

			SubscribeToBasicEvents(id);
			return true;
		}

		return false;
	}

	// Threaded function
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
		DeusCore::Logger::Instance()->Log(m_name, "End HandleInput");
	}

	/////////////////////////// 
	//          GAMES        // 
	///////////////////////////

	void WorldNetworkServer::GetGames(int clientId)
	{
		std::vector<unsigned int> gamesIds;
		for (DeusGames::iterator it = m_games.begin(); it != m_games.end(); ++it) {
			gamesIds.push_back(it->first);
		}

		std::unique_ptr<DeusCore::PacketGetGamesAnswer> p_packet = std::unique_ptr<DeusCore::PacketGetGamesAnswer>(new DeusCore::PacketGetGamesAnswer());
		p_packet->SetGames(gamesIds);

		SendPacket(std::move(p_packet), clientId, true);
	}

	void WorldNetworkServer::CreateGame(int clientId)
	{
		bool error = false;
		DeusClientConnections::iterator findClientIt = m_clientsConnections.find(clientId);
		if (findClientIt != m_clientsConnections.end())
		{
			/////////////
			// we don't need to listen the events anymore because
			// the game network server will do it, we are only intersted by disconnect event
			DeusServer::DeusEventDeleguate messageTcpReceivedDeleguate = fastdelegate::MakeDelegate(this, &WorldNetworkServer::ManageOnReceivedPacketEvent);
			findClientIt->second->RemoveListener(messageTcpReceivedDeleguate, DeusServer::DeusClient::DeusClientEventsType::OnTCPMessageReceived);

			m_games.insert(std::make_pair(m_nextGameId, std::unique_ptr<GameHandler>(new GameHandler(m_nextGameId))));

			/////////////
			// Let the gamehandler notify our client that he is connected to a game
			if (!m_games[m_nextGameId]->NewPlayer(findClientIt->first, findClientIt->second))
			{
				findClientIt->second->AddListener(messageTcpReceivedDeleguate, DeusServer::DeusClient::DeusClientEventsType::OnTCPMessageReceived);
				error = true;
			}
			else
				m_nextGameId++;
		}
		else
			error = true;

		if (error)
		{
			// Send error answer
			std::unique_ptr<DeusCore::PacketCreateGameAnswer> p_packet = std::unique_ptr<DeusCore::PacketCreateGameAnswer>(new DeusCore::PacketCreateGameAnswer());
			p_packet->SetSuccess(false);
			SendPacket(std::move(p_packet), clientId, true);
		}
	}

	void WorldNetworkServer::JoinGame(int clientId, int gameId)
	{
		bool error = false;
		DeusClientConnections::iterator findClientIt = m_clientsConnections.find(clientId);
		if (findClientIt != m_clientsConnections.end())
		{
			DeusGames::iterator findGameIt = m_games.find(gameId);
			if (findGameIt != m_games.end())
			{
				/////////////
				// we don't need to listen the events anymore because
				// the game network server will do it, we are only intersted by disconnect event
				DeusServer::DeusEventDeleguate messageTcpReceivedDeleguate = fastdelegate::MakeDelegate(this, &WorldNetworkServer::ManageOnReceivedPacketEvent);
				findClientIt->second->RemoveListener(messageTcpReceivedDeleguate, DeusServer::DeusClient::DeusClientEventsType::OnTCPMessageReceived);

				/////////////
				// Let the gamehandler notify our client that he is connected to a game
				if (!m_games[gameId]->NewPlayer(findClientIt->first, findClientIt->second))
				{
					error = true;
					findClientIt->second->AddListener(messageTcpReceivedDeleguate, DeusServer::DeusClient::DeusClientEventsType::OnTCPMessageReceived);
				}
			}
			else
				error = true;
		}
		else
			error = true;

		if (error)
		{
			// Send error answer
			std::unique_ptr<DeusCore::PacketJoinGameAnswer> p_packet = std::unique_ptr<DeusCore::PacketJoinGameAnswer>(new DeusCore::PacketJoinGameAnswer());
			p_packet->SetGame(gameId);
			p_packet->SetSuccess(false);
			SendPacket(std::move(p_packet), clientId, true);
		}
	}
}
