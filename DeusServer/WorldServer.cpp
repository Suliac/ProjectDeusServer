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
		StopConnections();
	}

	void WorldServer::Run(std::string ipAddr, std::string port)
	{
		std::thread m_inputHandlerThread = std::thread([this] { HandleInput(); });

		m_ipAddr = ipAddr;
		m_port = port;

		//Init listener
		DeusCore::Logger::Instance()->Log(m_name, "Init & Start tcp listener");
		m_listener.Init(m_ipAddr, m_port, true);
		m_listener.Start();

		size_t id = 1;
		while (!m_wantToStop)
		{
			if (AcceptConnection(id))
				id++;

			HandleDisconnection();

			if (m_wantToStop)
				break;
		}

		StopConnections();
		m_inputHandlerThread.join();

	}
	void WorldServer::StopConnections()
	{
		m_listener.SocketShutdown();
		m_listener.SocketClose();

		for (auto it = m_clientsConnections.begin(); it != m_clientsConnections.end(); ++it)
		{
			// create deleguate functions to retrieve old values !
			// so we don't have to cache the deleguate
			// Can be weird but it works
			DeusServer::DeusEventDeleguate messageTcpReceivedDeleguate = fastdelegate::MakeDelegate(this, &WorldServer::ManageOnReceivedPacketEvent);
			DeusServer::DeusEventDeleguate disconnectTcpDeleguate = fastdelegate::MakeDelegate(this, &WorldServer::ManageDisconnectedEvent);

			// TODO : Remove UDP listener too

			// remove deleguates
			it->second->RemoveListener(messageTcpReceivedDeleguate, DeusServer::DeusClient::DeusClientEventsType::OnTCPMessageReceived);
			it->second->RemoveListener(disconnectTcpDeleguate, DeusServer::DeusClient::DeusClientEventsType::OnTCPDisconnect);
		}

		m_clientsConnections.clear();
	}

	bool WorldServer::AcceptConnection(const size_t& id)
	{
		// create a pointer on empty tcp socket
		std::unique_ptr<DeusCore::TcpSocket> p_client = std::make_unique<DeusCore::TcpSocket>();

		// timeout is in the accept function
		// block until timeout (3sec currently)
		if (m_listener.Accept(*p_client))
		{

			DeusCore::Logger::Instance()->Log(m_name, "Connection accepted !");

			// we want to transfert the ownership of our TCP socket to our DeusClient instance
			// which handle the communication behavior for TCP (and UDP too in the futur)
			m_clientsConnections.insert(
				std::make_pair(
					id,
					std::unique_ptr<DeusServer::DeusClient>(new DeusServer::DeusClient(id, std::move(p_client)))));

			///////////////////// Manage deleguate 
			// create deleguate functions
			DeusServer::DeusEventDeleguate messageTcpReceivedDeleguate = fastdelegate::MakeDelegate(this, &WorldServer::ManageOnReceivedPacketEvent);
			DeusServer::DeusEventDeleguate disconnectTcpDeleguate = fastdelegate::MakeDelegate(this, &WorldServer::ManageDisconnectedEvent);

			// register deleguates with connection
			m_clientsConnections[id]->AddListener(messageTcpReceivedDeleguate, DeusServer::DeusClient::DeusClientEventsType::OnTCPMessageReceived);
			m_clientsConnections[id]->AddListener(disconnectTcpDeleguate, DeusServer::DeusClient::DeusClientEventsType::OnTCPDisconnect);


			// TODO : subscribe to UDP Events too
			return true;
		}

		return false;
	}

	void WorldServer::HandleDisconnection()
	{
		// Check if we have some disconnection 
		m_lockClientsToDisconnect.lock();
		if (m_clientToDisconnect.size() > 0)
		{
			// we don't want to manage all disconnection if we have to much
			size_t numberConnectionToDelete = m_clientToDisconnect.size() > MAX_DISCONNECT_PER_LOOP ? MAX_DISCONNECT_PER_LOOP : m_clientToDisconnect.size();
			for (int i = 0; i < numberConnectionToDelete; i++)
			{
				int idConnectionToDelete = m_clientToDisconnect.front();
				m_clientToDisconnect.pop();

				DeusClientConnections::iterator matchingConnectionIt = m_clientsConnections.find(idConnectionToDelete);
				if (matchingConnectionIt != m_clientsConnections.end())
				{
					///////// 1 - Remove listener
					// create deleguate functions to retrieve old values !
					// so we don't have to cache the deleguate
					// Can be weird but it works
					DeusServer::DeusEventDeleguate messageTcpReceivedDeleguate = fastdelegate::MakeDelegate(this, &WorldServer::ManageOnReceivedPacketEvent);
					DeusServer::DeusEventDeleguate disconnectTcpDeleguate = fastdelegate::MakeDelegate(this, &WorldServer::ManageDisconnectedEvent);
					
					// remove deleguates
					matchingConnectionIt->second->RemoveListener(messageTcpReceivedDeleguate, DeusServer::DeusClient::DeusClientEventsType::OnTCPMessageReceived);
					matchingConnectionIt->second->RemoveListener(messageTcpReceivedDeleguate, DeusServer::DeusClient::DeusClientEventsType::OnTCPDisconnect);

					///////// 2 - Delete connection
					m_clientsConnections.erase(matchingConnectionIt);
					DeusCore::Logger::Instance()->Log("World Server", "Client " + std::to_string(idConnectionToDelete) + " successfully deleted. There are now : " + std::to_string(m_clientsConnections.size()) + "client(s) connected");
				}
			}
		}
		m_lockClientsToDisconnect.unlock();
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


	// Warning : on those deleguate your are on the same execution thread than the one 
	// which raise the deleguates

	void WorldServer::ManageOnReceivedPacketEvent(int id, DeusCore::PacketSPtr p_packet)
	{
		DeusCore::Logger::Instance()->Log(m_name, "Received TCP message of type : " + std::to_string(p_packet->GetID()));
	}

	void WorldServer::ManageDisconnectedEvent(int id, DeusCore::PacketSPtr p_packet)
	{
		DeusClientConnections::iterator matchingConnectionIt = m_clientsConnections.find(id);

		if (matchingConnectionIt != m_clientsConnections.end())
		{
			// add the id of the client we want to delete
			m_lockClientsToDisconnect.lock();
			m_clientToDisconnect.push(matchingConnectionIt->first);
			m_lockClientsToDisconnect.unlock();
		}

		DeusCore::Logger::Instance()->Log("World Server", "Client " + std::to_string(id) + " disconnection handled in deleguate");
	}
}
