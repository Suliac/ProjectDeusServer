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

		void SendPacket(DeusCore::PacketSPtr p_packet, bool isTcp);
		void SetConnectionGameId(Id gameId);
		void SetUdpConnectionInitialized(const std::string& playerName) { 
			m_playerName = playerName;
			m_udpConnection.SetUdpConnectionInitialized();
		}

		std::string GetPlayerName() const { return m_playerName; }
	private:
		// TCP communication interface
		DeusTcpConnection m_tcpConnection;

		// UDP communication interface
		DeusUdpConnection m_udpConnection;

		// ID of the connection
		Id m_id;

		std::string m_playerName;
	};

	using DeusClientUPtr = std::unique_ptr<DeusServer::DeusClient>;
	using DeusClientSPtr = std::shared_ptr<DeusServer::DeusClient>;
}

