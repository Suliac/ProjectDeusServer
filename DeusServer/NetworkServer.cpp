#include "NetworkServer.h"

#include "DeusCore/Logger.h"
#include "DeusCore/FastDelegate.h"

namespace DeusServer
{
	NetworkServer::NetworkServer()
	{
	}


	NetworkServer::~NetworkServer()
	{
	}

	void NetworkServer::Run() {
		std::thread processPacketThread = std::thread([this] { ProcessPackets(); });
		
		OnStart();

		while (!m_wantToStop)
		{
			OnUpdate();
			HandleDisconnection();

			if (m_wantToStop)
				break;
		}
		DeusCore::Logger::Instance()->Log(m_name, "End Run");

		m_newPacketBlocker.notify_all();

		StopConnections();
		
		if (processPacketThread.joinable())
			processPacketThread.join();

		OnEnd();
	}

	void NetworkServer::StopConnections()
	{
		for (auto it = m_clientsConnections.begin(); it != m_clientsConnections.end(); ++it)
		{
			// create deleguate functions to retrieve old values !
			// so we don't have to cache the deleguate
			// Can be weird but it works
			DeusServer::DeusEventDeleguate messageTcpReceivedDeleguate = fastdelegate::MakeDelegate(this, &NetworkServer::ManageOnReceivedPacketEvent);
			DeusServer::DeusEventDeleguate disconnectTcpDeleguate = fastdelegate::MakeDelegate(this, &NetworkServer::ManageDisconnectedEvent);

			// TODO : Remove UDP listener too

			// remove deleguates
			it->second->RemoveListener(messageTcpReceivedDeleguate, DeusServer::DeusClient::DeusClientEventsType::OnTCPMessageReceived);
			it->second->RemoveListener(disconnectTcpDeleguate, DeusServer::DeusClient::DeusClientEventsType::OnTCPDisconnect);
		}

		m_clientsConnections.clear();
	}

	bool NetworkServer::SubscribeToBasicEvents(int clientId)
	{
		DeusClientConnections::iterator findIt = m_clientsConnections.find(clientId);
		if (findIt == m_clientsConnections.end())
			return false;

		///////////////////// Manage deleguate 
		// create deleguate functions
		DeusServer::DeusEventDeleguate messageTcpReceivedDeleguate = fastdelegate::MakeDelegate(this, &NetworkServer::ManageOnReceivedPacketEvent);
		DeusServer::DeusEventDeleguate disconnectTcpDeleguate = fastdelegate::MakeDelegate(this, &NetworkServer::ManageDisconnectedEvent);

		// register deleguates with connection
		m_clientsConnections[clientId]->AddListener(messageTcpReceivedDeleguate, DeusServer::DeusClient::DeusClientEventsType::OnTCPMessageReceived);
		m_clientsConnections[clientId]->AddListener(disconnectTcpDeleguate, DeusServer::DeusClient::DeusClientEventsType::OnTCPDisconnect);
	}

	void NetworkServer::HandleDisconnection()
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
					DeusServer::DeusEventDeleguate messageTcpReceivedDeleguate = fastdelegate::MakeDelegate(this, &NetworkServer::ManageOnReceivedPacketEvent);
					DeusServer::DeusEventDeleguate disconnectTcpDeleguate = fastdelegate::MakeDelegate(this, &NetworkServer::ManageDisconnectedEvent);

					// remove deleguates
					matchingConnectionIt->second->RemoveListener(messageTcpReceivedDeleguate, DeusServer::DeusClient::DeusClientEventsType::OnTCPMessageReceived);
					matchingConnectionIt->second->RemoveListener(messageTcpReceivedDeleguate, DeusServer::DeusClient::DeusClientEventsType::OnTCPDisconnect);

					///////// 2 - Delete connection
					m_clientsConnections.erase(matchingConnectionIt);
					DeusCore::Logger::Instance()->Log(m_name, "Client " + std::to_string(idConnectionToDelete) + " successfully deleted. There are now : " + std::to_string(m_clientsConnections.size()) + "client(s) connected");
				}
			}
		}
		m_lockClientsToDisconnect.unlock();
	}

	// Threaded function
	void NetworkServer::ProcessPackets()
	{
		while (!m_wantToStop)
		{
			std::pair<int, DeusCore::PacketSPtr> packetToProcess;
			bool getPacket = false;

			std::unique_lock<std::mutex> lk(m_lockPacketsToProcess);
			// wait for a packet to be added to our queue 
			// or if there already is packet to process, don't wait
			m_newPacketBlocker.wait(lk, [this] {return m_thereIsPacket || m_wantToStop; });
			if (m_wantToStop)
			{
				lk.unlock();
				break;
			}

			// Get packet from queue
			if (!m_packetsToProcess.empty())
			{
				getPacket = true;

				// Pop the front packet to use it later without blocking 
				// for too long the access on the queue
				packetToProcess = m_packetsToProcess.front();
				m_packetsToProcess.pop();

				// update our thereIsPacket var for our condition_var
				if (!m_packetsToProcess.empty())
					m_thereIsPacket = true;
				else
					m_thereIsPacket = false;

			}

			lk.unlock();

			// Process packet
			if (getPacket)
			{
				OnInterpretPacket(packetToProcess.first, packetToProcess.second);
			}
		}
		DeusCore::Logger::Instance()->Log(m_name, "End ProcessPackets");

	}

	bool NetworkServer::SendPacket(DeusCore::PacketUPtr&& p_packet, int clientId, bool sendTcp)
	{
		DeusClientConnections::iterator findClientIt = m_clientsConnections.find(clientId);
		if (findClientIt != m_clientsConnections.end())
		{
			findClientIt->second->SendPacket(std::move(p_packet), sendTcp);
			return true;
		}

		return false;
	}

	/////////////////////////// 
	//       DELEGUATES      // 
	/////////////////////////// 
	// Warning : on those deleguate your are on the same execution thread than the one 
	// which raise the deleguates

	void NetworkServer::ManageOnReceivedPacketEvent(int idSender, DeusCore::PacketSPtr p_packet)
	{
		DeusCore::Logger::Instance()->Log(m_name, "Received TCP message of type : " + std::to_string(p_packet->GetID()));

		m_lockPacketsToProcess.lock();

		m_packetsToProcess.push(std::make_pair(idSender, p_packet));
		m_thereIsPacket = true;

		m_lockPacketsToProcess.unlock();
		m_newPacketBlocker.notify_all();

		DeusCore::Logger::Instance()->Log(m_name, "Message added to queue");
	}

	void NetworkServer::ManageDisconnectedEvent(int idSender, DeusCore::PacketSPtr p_packet)
	{
		DeusClientConnections::iterator matchingConnectionIt = m_clientsConnections.find(idSender);

		if (matchingConnectionIt != m_clientsConnections.end())
		{
			// add the id of the client we want to delete
			m_lockClientsToDisconnect.lock();
			m_clientToDisconnect.push(matchingConnectionIt->first);
			m_lockClientsToDisconnect.unlock();
		}

		DeusCore::Logger::Instance()->Log(m_name, "Client " + std::to_string(idSender) + " disconnection handled in deleguate");
	}
}
