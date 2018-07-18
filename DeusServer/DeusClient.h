#pragma once
#include <queue>
#include <thread>
#include <mutex>
#include <memory>

#include "DeusCore/Packet.h"
#include "DeusCore/TcpSocket.h"

namespace DeusServer
{
	class DeusClient
	{
	public:
		DeusClient(const DeusNetwork::TcpSocket& clientSocket);
		~DeusClient();

		void AddPacketToQueue(DeusNetwork::PacketUPtr& p_packet);
	private:
		void ThreadSendAndReceiveTCP();

		bool TryTakePacket(DeusNetwork::PacketUPtr& p_packet);

		std::thread m_clientTCPThread;
		//std::thread m_clientUDPThread;
		bool m_cancellationRequested = false;
		
		std::queue<DeusNetwork::PacketUPtr> m_packetsToSend;
		
		DeusNetwork::TcpSocket m_clientTCPConnection;
		//DeusNetwork::UdpSocket m_clientUDPConnection;

		//mutex on packet's queue
		std::mutex m_packetQueueLock;
	};
}

