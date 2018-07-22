#pragma once
#include "DeusCore/Packet.h"
#include "DeusClient.h"

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

		void Run();
	protected:
		/****************** RUN *********************/

		// Execute at each tick of Run()
		virtual void OnUpdate() = 0;

		// Execute before the loop of Run()
		virtual void OnStart() = 0;
		
		// Execute after the loop of Run()
		virtual void OnEnd() = 0;

		// Poll client id from queue of client to disconnect 
		// and delete them from the active clients
		void HandleDisconnection();

		// clear client connection after remove deleguate
		void StopConnections();


		/********************************************/

		/*****************  PACKETS *****************/

		// Get the paquets from the queue and interpret them
		void ProcessPackets();

		// The way we want our server to interpret packet we get in ProcessPackets()
		virtual void OnInterpretPacket(int senderId, DeusCore::PacketSPtr p_packet) = 0;

		// Send Packet to a client
		bool SendPacket(DeusCore::PacketUPtr&& p_packet, int clientId, bool sendTcp);

		/********************************************/

		/**************** DELEGUATES ****************/

		void ManageOnReceivedPacketEvent(int idSender, DeusCore::PacketSPtr p_packet);
		void ManageDisconnectedEvent(int idSender, DeusCore::PacketSPtr p_packet);

		// Subscribe to onmessage and disconnect event
		bool SubscribeToBasicEvents(int clientId);
		/********************************************/

		// Save all the client connection accepted with an id
		DeusClientConnections m_clientsConnections;

		// Queue of clients id to disconnect
		std::queue<int> m_clientToDisconnect;

		// Mutex on queue as it is used by differents threads
		std::mutex m_lockClientsToDisconnect;

		// Queue of packet to interpret
		std::queue<std::pair<int, DeusCore::PacketSPtr>> m_packetsToProcess;

		// Mutex on queue for packet to interpret as it is used by differents threads
		std::mutex m_lockPacketsToProcess;

		std::condition_variable m_newPacketBlocker;
		bool m_thereIsPacket = false;

		// Max disconnect per loop of the world server 
		// To avoid to lock for too long the queue
		const size_t MAX_DISCONNECT_PER_LOOP = 5;

		// Do we want to stop the server ?
		bool m_wantToStop = false;

		// debug name
		std::string m_name;
	};
}

