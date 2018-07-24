#pragma once
#include "DeusCore/Packet.h"
#include "DeusClient.h"
#include "DeusCore/EventManagerHandler.h"

#include <map>
#include <queue>
namespace DeusServer
{
	using DeusClientConnections = std::map<int, std::shared_ptr<DeusServer::DeusClient>>;
	
	class NetworkServer
	{
	public:
		NetworkServer();
		~NetworkServer();

		void Start(unsigned int gameId);
		void Stop();
		void WaitForStop();

		bool IsStop() { return m_isStop; }

	protected:
		// Execute during the Start()
		virtual void OnStart() = 0;

		// Execute during the Stop()
		virtual void OnStop() = 0;
		
		// To execute by the childrens class when a client disconnects
		virtual void OnDisconnectClient(int clientId) = 0;

		// The way we want our server to interpret packet we get in ProcessPackets()
		//virtual void OnInterpretPacket(int senderId, DeusCore::PacketSPtr p_packet) = 0;
		void DisconnectDeleguate(DeusCore::DeusEventSPtr p_packet);

		// Send Packet to a client
		bool SendPacket(DeusCore::PacketUPtr&& p_packet, int clientId, bool sendTcp);

		/********************************************/

		// Do we want to stop the server ?
		bool m_wantToStop = false;
		
		// Id of the game : 0 for the world server, 1 for the game server #1, 2 for the #2 and so on
		unsigned int m_gameId;

		// Save all the client connection accepted with an id
		DeusClientConnections m_clientsConnections;
		
		// Locker for the access of the client connections
		std::recursive_mutex m_lockClients;

		// debug name
		std::string m_name;
	private:
		bool m_isStop = false;

		/////////////////////////// 
		//       DISCONNECT      // 
		///////////////////////////
		std::thread m_disconnectThread;

		// Poll client id from queue of client to disconnect 
		// and delete them from the active clients
		void HandleDisconnect();

		std::condition_variable m_newDisconnectBlocker;
		bool m_thereIsDisconnect = false;

		// Mutex on queue as it is used by differents threads
		std::mutex m_lockClientsToDisconnect;

		// Queue of clients id to disconnect
		std::queue<int> m_clientToDisconnect;

		// Max time we try to handle disconnect
		// To avoid to lock for too long the queue
		const unsigned long MAX_DISCONNECT_TIME_MS = 1000;
	};
}

