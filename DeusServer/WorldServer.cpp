#include "DeusCore\DeusSocketException.h"
#include "DeusCore\Logger.h"

#include "WorldServer.h"

#include <stdio.h>


namespace DeusServer
{
	WorldServer::WorldServer()
	{
	}

	WorldServer::~WorldServer()
	{
		clientsConnections.clear();
	}

	void WorldServer::Run(std::string ipAddr, std::string port)
	{
		std::thread m_inputHandlerThread = std::thread([this] { HandleInput(); });

		m_ipAddr = ipAddr;
		m_port = port;

		//Init listener
		DeusCore::Logger::Instance()->Log(m_name, "Init & Start tcp listener");
		listener.Init(m_ipAddr, m_port, true);
		listener.Start();

		size_t id = 1;
		while (!m_wantToStop)
		{
			// create a pointer on empty tcp socket
			std::unique_ptr<DeusCore::TcpSocket> p_client = std::make_unique<DeusCore::TcpSocket>();
			
			do {

				if (m_wantToStop)
					break;

			} while (!listener.Accept(*p_client)); // timeout is in the accept function
			
			if (m_wantToStop)
				break;

			DeusCore::Logger::Instance()->Log(m_name, "Connection accepted !");
			
			// we want to transfert the ownership of our TCP socket to our DeusClient instance
			// which handle the communication behavior for TCP (and UDP too in the futur)
			clientsConnections.insert(
				std::make_pair(
					id,
					std::unique_ptr<DeusServer::DeusClient>(new DeusServer::DeusClient(id, std::move(p_client)))));
			
			// TODO : subscribe to OnReceiveMessage Event
			/*
			// create deleguate functions
			DeusServer::DeusEventDeleguate messageTcpReceivedDeleguate = fastdelegate::MakeDelegate(this, &DeusServer::DeusClient::ManageTcpMessageReceivedEvent);
			DeusServer::DeusEventDeleguate disconnectTcpDeleguate = fastdelegate::MakeDelegate(this, &DeusServer::DeusClient::ManageTcpDisconnectedEvent);

			// register deleguates with connection
			m_tcpConnection.AddListener(messageTcpReceivedDeleguate, DeusConnection::DeusConnectionEventsType::OnMessageReceived);
			m_tcpConnection.AddListener(disconnectTcpDeleguate, DeusConnection::DeusConnectionEventsType::Disconnected);
			*/

			//clientsConnections[id]->AddListener()

			id++;
		}

		StopConnections();
		m_inputHandlerThread.join();

	}
	void WorldServer::StopConnections()
	{
		listener.SocketShutdown();
		listener.SocketClose();

		clientsConnections.clear();
	}

	// Threaded function
	void WorldServer::HandleInput()
	{
		while (!m_wantToStop)
		{
			// Get keyboard input
			char letter;
			letter = getchar();

			if (letter == 'q')
			{
				DeusCore::Logger::Instance()->Log(m_name, "Stop requested");
				m_wantToStop = true;
			}

		}
	}
}
