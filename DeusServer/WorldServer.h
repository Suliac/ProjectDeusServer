#pragma once
#include "DeusCore/TcpListener.h"

#include "DeusClient.h"

#include <string>
#include <map>
#include <thread>

namespace DeusServer
{
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
		// Ip adress of the server
		std::string m_ipAddr;

		// Port of the server
		std::string m_port;

		// Socket that listen TCP connection
		DeusCore::TcpListener listener;

		// Save all the client connection accepted with an id
		std::map<int, std::unique_ptr<DeusServer::DeusClient>> clientsConnections;
	
		bool m_wantToStop = false;

		// debug name
		const std::string m_name = "World Server";
	};
}

