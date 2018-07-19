#pragma once
#include <queue>
#include <thread>
#include <mutex>
#include <memory>

#include "DeusTcpConnection.h"

namespace DeusServer
{
	class DeusClient
	{
	public:
		enum class DeusClientEventsType {
			OnTCPMessageReceived = 0x0,
			OnTCPDisconnect = 0x1,
			OnUDPMessageReceived = 0x2,
			OnUDPDisconnect = 0x3
		};
		using DeusClientEventDeleguateMap = std::map<DeusClientEventsType, DeusEventDeleguateVector>;

		DeusClient(int id, std::unique_ptr<DeusNetwork::TcpSocket> communicationSocket);
		~DeusClient();
		
		// Just access to specific connection events
		bool AddListener(const DeusEventDeleguate& eventDeleguate, const DeusClientEventsType type);

		// Just access to specific connection events
		bool RemoveListener(const DeusEventDeleguate& eventDeleguate, const DeusClientEventsType type);
	private:
		// TCP communication interface
		DeusTcpConnection m_tcpConnection;

		// ID of the connection
		int m_id;

		void ManageTcpMessageReceivedEvent(int id, DeusNetwork::PacketSPtr p_packet);
		void ManageTcpDisconnectedEvent(int id, DeusNetwork::PacketSPtr p_packet);
	};
}

