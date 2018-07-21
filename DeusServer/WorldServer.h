#pragma once
#include "DeusCore/TcpListener.h"

#include "DeusClient.h"

#include <string>
#include <map>
#include <thread>

namespace DeusServer
{
	using DeusClientConnections = std::map<int, std::unique_ptr<DeusServer::DeusClient>>;

	class WorldServer
	{
	public:
		WorldServer();
		~WorldServer();

		void Run(std::string ipAddr = "127.0.0.1", std::string port = "27015");

	private:

		/////////////////////////// 
		//         SERVER        // 
		/////////////////////////// 

		// Accept new connection
		bool AcceptConnection(const unsigned int id, const unsigned int timeoutSecond = DEFAULT_SOCKETSTATE_TIMEOUT);

		// Poll client id from queue of client to disconnect 
		// and delete them from the active clients
		void HandleDisconnection();

		// stop listener and clear client connection after remove deleguate
		void StopConnections();

		// Threaded function to accept users input
		void HandleInput();

		// threaded function to  interpret messages
		void ProcessPackets();
		
		/////////////////////////// 
		//          GAMES        // 
		///////////////////////////
		void GetGames();
		void CreateGame();
		void JoinGame();

		/////////////////////////// 
		//       DELEGUATES      // 
		/////////////////////////// 

		void ManageOnReceivedPacketEvent(int idSender, DeusCore::PacketSPtr p_packet);
		void ManageDisconnectedEvent(int idSender, DeusCore::PacketSPtr p_packet);

		/**************** Attributes ********************/

		/////////////////////////// 
		//         SERVER        // 
		/////////////////////////// 

		// Ip adress of the server
		std::string m_ipAddr;

		// Port of the server
		std::string m_port;

		// Socket that listen TCP connection
		DeusCore::TcpListener m_listener;

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

		/////////////////////////// 
		//          GAMES        // 
		///////////////////////////

		std::map<int, std::string> m_games;

		/************************************/

		// debug name
		const std::string m_name = "World Server";
	};
}

