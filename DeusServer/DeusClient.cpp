
#include "DeusClient.h"
namespace DeusServer
{
	//DeusClient::DeusClient(const DeusNetwork::TcpSocket& clientTcpSocket /*, const DeusNetwork::UdpSocket& clientUdpSocket*/)
	//{
	//	m_clientTCPConnection = clientTcpSocket;

	//	m_clientTCPThread = std::thread(&DeusClient::ThreadSendAndReceiveTCP, this); // init thread only here
	//}

	//DeusClient::~DeusClient()
	//{
	//	m_cancellationRequested = true;
	//	m_clientTCPThread.join();
	//}

	//void DeusClient::AddPacketToQueue(const DeusNetwork::Packet & packet)
	//{
	//	m_packetQueueLock.lock();
	//	m_packetsToSend.push(packet);
	//	m_packetQueueLock.unlock();
	//}

	//void DeusClient::ThreadSendAndReceiveTCP(std::string test)
	//{
	//	while (m_cancellationRequested)
	//	{
	//		// 1- We first try to send our message
	//		std::unique_ptr<DeusNetwork::Packet> p_toSendPacket;
	//		while (TryTakePacket(p_toSendPacket))
	//		{
	//			if (p_toSendPacket)
	//			{
	//				DeusNetwork::Buffer512 buffer;
	//				DeusNetwork::Packet::Serialize(buffer, *(p_toSendPacket.get()));
	//				size_t byteSent = 0;

	//				m_clientTCPConnection.TCPSend(*(p_toSendPacket), byteSent);
	//			}
	//		}

	//		// 2- Then if there is data to receive, get them
	//	}
	//}

	//bool DeusClient::TryTakePacket(std::unique_ptr<DeusNetwork::Packet>& p_packet)
	//{
	//	bool popedElement = false;
	//	m_packetQueueLock.lock();

	//	if (m_packetsToSend.size() > 0)
	//	{
	//		p_packet = std::make_unique<DeusNetwork::Packet>(std::move(m_packetsToSend.front()));
	//		m_packetsToSend.pop();

	//		popedElement = true;
	//	}

	//	if (p_packet == nullptr)
	//		return false;

	//	m_packetQueueLock.unlock();
	//	return popedElement;
	//}

}
