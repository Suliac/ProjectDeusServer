#pragma once
#include <queue>
#include <thread>
#include <mutex>
#include <memory>

#include "DeusCore/Packet.h"
#include "DeusCore/TcpSocket.h"

namespace DeusServer
{
	//class DeusClient
	//{
	//public:
	//	DeusClient(const DeusNetwork::TcpSocket& clientSocket);
	//	~DeusClient();

	//	void AddPacketToQueue(const DeusNetwork::Packet& packet);
	//private:
	//	void ThreadSendAndReceiveTCP(std::string test);

	//	bool TryTakePacket(std::unique_ptr<DeusNetwork::Packet>& p_packet);

	//	std::thread m_clientTCPThread;
	//	//std::thread m_clientUDPThread;
	//	bool m_cancellationRequested = false;
	//	
	//	std::queue<DeusNetwork::Packet> m_packetsToSend;
	//	
	//	DeusNetwork::TcpSocket m_clientTCPConnection;
	//	//DeusNetwork::UdpSocket m_clientUDPConnection;

	//	//mutex on packet's queue
	//	std::mutex m_packetQueueLock;
	//};
}

