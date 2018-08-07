#pragma once
#include "DeusCore/Packet.h"
#include "DeusClient.h"
#include "DeusCore/EventManagerHandler.h"

#include <map>
#include <queue>
namespace DeusServer
{
	using DeusClientConnections = std::map<Id, std::shared_ptr<DeusServer::DeusClient>>;

	class NetworkServer
	{
	public:
		NetworkServer();
		~NetworkServer();

		// Init and launch the disconnect handler thread
		void Start(Id gameId);

		// Wait for stop request then Stop() the server
		void WaitForStop();
		
		// Stop the server
		void RequestStop();
	protected:

		// Execute during the Start()
		virtual void OnStart() {};

		// Execute during the Stop()
		virtual void OnStop() {};
		virtual void OnLateStop() {};

		// To execute by the childrens class when a client disconnects
		virtual void OnDisconnectClient(Id clientId) {};

		// The way we want our server to interpret packet we get in ProcessPackets()
		//virtual void OnInterpretPacket(int senderId, DeusCore::PacketSPtr p_packet) = 0;
		void DisconnectDeleguate(DeusCore::DeusEventSPtr p_packet);

		// Send Packet to a client
		bool SendPacket(DeusCore::PacketUPtr&& p_packet, Id clientId, bool sendTcp);

		bool GetWantToStop() const { return m_wantToStop; };

		/********************************************/

		// Id of the game : 0 for the world server, 1 for the game server #1, 2 for the #2 and so on
		Id m_gameId;

		// Save all the client connection accepted with an id
		DeusClientConnections m_clientsConnections;

		// Locker for the access of the client connections
		std::recursive_mutex m_lockClients;

		// debug name
		std::string m_name;

	private:
		
		// Poll client id from queue of client to disconnect 
		// and delete them from the active clients
		void HandleDisconnect();

		// Wait for a RequestStop()
		void HandleStop();

		void Stop();

		/////////////////////////// 
		//       DISCONNECT      // 
		///////////////////////////
		std::thread m_stopServerThread;
		
		bool m_isStop = false;
		
		std::condition_variable m_waitForStopBlocker; // For HandleStop()
		std::condition_variable m_isStopBlocker; // for WaitToStop()
		std::mutex m_wantToStopLocker;
		std::mutex m_isStopLocker;

		// Do we want to stop the server ?
		bool m_wantToStop = false;

		/////////////////////////// 
		//       DISCONNECT      // 
		///////////////////////////
		std::thread m_disconnectThread;

		std::condition_variable m_newDisconnectBlocker;
		bool m_thereIsDisconnect = false;

		// Mutex on queue as it is used by differents threads
		std::mutex m_lockClientsToDisconnect;

		// Queue of clients id to disconnect
		std::queue<Id> m_clientToDisconnect;

		// Max time we try to handle disconnect
		// To avoid to lock for too long the queue
		const unsigned long MAX_DISCONNECT_TIME_MS = 1000;
	};
}

