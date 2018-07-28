#pragma once
#include <queue>
#include <thread>
#include <mutex>
#include <memory>

#include "DeusTcpConnection.h"
#include "DeusUdpConnection.h"

namespace DeusServer
{
	class DeusClient
	{
	public:
		DeusClient(int id, std::unique_ptr<DeusCore::TcpSocket> communicationSocket, const std::string& servAddr, const std::string& servPort);
		~DeusClient();
		
		void SendPacket(DeusCore::PacketUPtr p_packet, bool isTcp);
		void SetConnectionGameId(unsigned int gameId);

	private:
		// TCP communication interface
		DeusTcpConnection m_tcpConnection;

		// UDP communication interface
		DeusUdpConnection m_udpConnection;

		// ID of the connection
		int m_id;
	};

	using DeusClientUPtr = std::unique_ptr<DeusServer::DeusClient>;
	using DeusClientSPtr = std::shared_ptr<DeusServer::DeusClient>;
}

