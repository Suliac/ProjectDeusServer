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

	protected:
		void StopConnections();

		void HandleInput();
		
	private:

		/////////////////////////// 
		//         SERVER        // 
		/////////////////////////// 

		bool AcceptConnection(const size_t& id);
		void HandleDisconnection();
		

		/////////////////////////// 
		//       DELEGUATES      // 
		/////////////////////////// 

		void ManageOnReceivedPacketEvent(int id, DeusCore::PacketSPtr p_packet);
		void ManageDisconnectedEvent(int id, DeusCore::PacketSPtr p_packet);

		///////////////////////////

		// Ip adress of the server
		std::string m_ipAddr;

		// Port of the server
		std::string m_port;

		// Socket that listen TCP connection
		DeusCore::TcpListener m_listener;

		// Save all the client connection accepted with an id
		DeusClientConnections m_clientsConnections;

		std::queue<int> m_clientToDisconnect;
		std::mutex m_lockClientsToDisconnect;
		const size_t MAX_DISCONNECT_PER_LOOP = 5;
	
		bool m_wantToStop = false;

		// debug name
		const std::string m_name = "World Server";
	};
}

