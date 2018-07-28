#include "NetworkServer.h"

#include "DeusCore/Logger.h"
#include "DeusCore/DeusException.h"
#include "DeusCore/FastDelegate.h"

namespace DeusServer
{
	NetworkServer::NetworkServer()
	{
		m_gameId = 0;
	}

	//---------------------------------------------------------------------------------
	NetworkServer::~NetworkServer()
	{
		m_newDisconnectBlocker.notify_one(); 
		if (m_disconnectThread.joinable())
			m_disconnectThread.join();

		m_waitForStopBlocker.notify_one();
		if (m_stopServerThread.joinable())
			m_stopServerThread.join();

		if (!m_isStop)
		{
			DeusCore::DeusEventDeleguate disconnectDeleguate = fastdelegate::MakeDelegate(this, &NetworkServer::DisconnectDeleguate);
			DeusCore::EventManagerHandler::Instance()->RemoveListener(m_gameId, disconnectDeleguate, DeusCore::Packet::EMessageType::Disconnect);

			// then clear connections
			m_clientsConnections.clear();

			DeusCore::EventManagerHandler::Instance()->StopGameChannel(m_gameId);
			m_isStop = true;
		}
		m_isStopBlocker.notify_one();
	}

	//---------------------------------------------------------------------------------
	void NetworkServer::Start(unsigned int gameId)
	{
		m_gameId = gameId;

		//init threads
		m_disconnectThread = std::thread([this] { HandleDisconnect(); });
		m_stopServerThread = std::thread([this] { HandleStop(); });

		// Start event queue channel
		DeusCore::EventManagerHandler::Instance()->StartGameChannel(m_gameId);

		//Init deleguate
		DeusCore::DeusEventDeleguate disconnectDeleguate = fastdelegate::MakeDelegate(this, &NetworkServer::DisconnectDeleguate);
		DeusCore::EventManagerHandler::Instance()->AddListener(m_gameId, disconnectDeleguate, DeusCore::Packet::EMessageType::Disconnect);

		OnStart();

	}

	//---------------------------------------------------------------------------------
	// Called by HandleStop() threaded function
	//---------------------------------------------------------------------------------
	void NetworkServer::Stop()
	{
		m_wantToStop = true;

		if (!m_isStop)
		{
			// call children OnStop() if they have any specific behavior
			OnStop();

			// Wait for the end of disconnect handler thread
			m_newDisconnectBlocker.notify_one();
			if (m_disconnectThread.joinable())
				m_disconnectThread.join();

			DeusCore::DeusEventDeleguate disconnectDeleguate = fastdelegate::MakeDelegate(this, &NetworkServer::DisconnectDeleguate);
			DeusCore::EventManagerHandler::Instance()->RemoveListener(m_gameId, disconnectDeleguate, DeusCore::Packet::EMessageType::Disconnect);

			// then clear connections
			m_clientsConnections.clear();

			DeusCore::EventManagerHandler::Instance()->StopGameChannel(m_gameId);

			// call late children Stop function
			OnLateStop();

			m_isStop = true;
		}

	}

	//---------------------------------------------------------------------------------
	void NetworkServer::WaitForStop()
	{
		std::unique_lock<std::mutex> lk(m_isStopLocker);
		m_isStopBlocker.wait(lk, [this] {return m_isStop; });
		lk.unlock();
	}

	//---------------------------------------------------------------------------------
	// Threaded
	//---------------------------------------------------------------------------------
	void NetworkServer::HandleStop()
	{
		std::unique_lock<std::mutex> lk(m_wantToStopLocker); // <------ LOCK
															 // wait for any request to stop server
		m_waitForStopBlocker.wait(lk, [this] {return m_wantToStop; });

		Stop();

		lk.unlock(); // <------------  UNLOCK
		m_isStopBlocker.notify_all(); // notify WaitForStop

		DeusCore::Logger::Instance()->Log(m_name, "End HandleStop() Thread");
	}

	//---------------------------------------------------------------------------------
	// Threaded
	//---------------------------------------------------------------------------------
	void NetworkServer::HandleDisconnect()
	{
		while (!m_wantToStop)
		{
			std::unique_lock<std::mutex> lk(m_lockClientsToDisconnect); // <------ LOCK
			// wait for a packet to be added to our queue
			// or if there already is packet to process, don't wait
			m_newDisconnectBlocker.wait(lk, [this] {return m_thereIsDisconnect || m_wantToStop; }); // <------------ DISCONNECT LOCK
			if (m_wantToStop)
			{
				lk.unlock();
				break;
			}

			unsigned long currentMs = GetTickCount();
			unsigned long maxMs = currentMs + MAX_DISCONNECT_TIME_MS;

			// Check if we have some disconnection
			while (!m_clientToDisconnect.empty())
			{
				int idConnectionToDelete = m_clientToDisconnect.front();
				m_clientToDisconnect.pop();

				m_lockClients.lock();// <------------ CLIENT LOCK
				DeusClientConnections::iterator matchingConnectionIt = m_clientsConnections.find(idConnectionToDelete);
				if (matchingConnectionIt != m_clientsConnections.end())
				{
					// Delete connection
					m_clientsConnections.erase(matchingConnectionIt);
					DeusCore::Logger::Instance()->Log(m_name, "Client " + std::to_string(idConnectionToDelete) + " successfully deleted. There are now : " + std::to_string(m_clientsConnections.size()) + "client(s) connected");

					// Disconnect specific code
					OnDisconnectClient(idConnectionToDelete); // we know that idConnectionToDelete exist
				}
				m_lockClients.unlock(); // <------------ CLIENT UNLOCK

				if (currentMs > maxMs) // ran out of time
					break;
			}
			m_thereIsDisconnect = false;
			lk.unlock(); // <------------ DISCONNECT QUEUE UNLOCK
		}
		DeusCore::Logger::Instance()->Log(m_name, "End HandleDisconnect Thread");
	}

	//---------------------------------------------------------------------------------
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

	//---------------------------------------------------------------------------------
	void NetworkServer::RequestStop()
	{
		m_wantToStopLocker.lock(); // <----------- LOCK
		m_wantToStop = true;
		m_wantToStopLocker.unlock(); // <----------- UNLOCK
		m_waitForStopBlocker.notify_one();
	}

	/////////////////////////// 
	//       DELEGUATES      // 
	/////////////////////////// 
	// Warning : on those deleguate your are on the same execution thread than the one 
	// which raise the deleguates

	//---------------------------------------------------------------------------------
	void NetworkServer::DisconnectDeleguate(DeusCore::DeusEventSPtr p_packet)
	{
		m_lockClientsToDisconnect.lock();

		m_clientToDisconnect.push(p_packet->first);
		m_thereIsDisconnect = true;

		m_lockClientsToDisconnect.unlock();
		m_newDisconnectBlocker.notify_one();

		DeusCore::Logger::Instance()->Log(m_name, "Disconnect added to queue");
	}
}
