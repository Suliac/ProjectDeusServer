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
		DeusClient(Id id, std::unique_ptr<DeusCore::TcpSocket> communicationSocket, const std::string& servAddr, const std::string& servPort);
		~DeusClient();

		void SendPacket(DeusCore::PacketUPtr p_packet, bool isTcp);
		void SetConnectionGameId(Id gameId);

	private:
		// TCP communication interface
		DeusTcpConnection m_tcpConnection;

		// UDP communication interface
		DeusUdpConnection m_udpConnection;

		// ID of the connection
		Id m_id;
	};

	using DeusClientUPtr = std::unique_ptr<DeusServer::DeusClient>;
	using DeusClientSPtr = std::shared_ptr<DeusServer::DeusClient>;
}

