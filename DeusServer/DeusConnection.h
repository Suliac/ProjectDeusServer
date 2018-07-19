#pragma once
#include <thread>
#include <queue>
#include <mutex>

#include "DeusCore/Packet.h"

namespace DeusServer
{
#define DEFAULT_DEUSCLIENT_BUFFER_SIZE 2048
	class DeusConnection
	{
	public:
		void AddPacketToQueue(DeusNetwork::PacketUPtr& p_packet);

	protected:
		virtual void ThreadSendAndReceive() = 0;
		bool TryTakePacket(DeusNetwork::PacketUPtr& p_packet);

		// Send/Receive thread
		std::thread m_communicationThread;

		// Queue of packets to send
		std::queue<DeusNetwork::PacketUPtr> m_packetsToSend;

		//mutex on packet's queue
		std::mutex m_packetQueueLock;

		DeusNetwork::Buffer512 writeBuffer, readBuffer, deserializeBuffer;
		std::vector<char> allByteReceivedBuffer;
		size_t sentByteCount, readedByteCount;

		// Do we want to stop the connection ?
		bool m_cancellationRequested = false;

	};
}


