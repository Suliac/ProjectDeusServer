#pragma once
#include <thread>
#include <queue>
#include <mutex>
#include <map>

#include "DeusCore/Packet.h"
#include "DeusCore/FastDelegate.h"

namespace DeusServer
{
	// FastDelegate2<connection id (int), shared_ptr on a packet>
	using DeusClientEventDeleguate = fastdelegate::FastDelegate2<int, DeusCore::PacketSPtr>;
	using DeusClientEventDeleguateVector = std::vector<DeusClientEventDeleguate>;


#define DEFAULT_DEUSCLIENT_BUFFER_SIZE 2048
	class DeusConnection
	{
	public:
		
		DeusConnection(int id) { 
			m_id = id;
			m_gameId = 0;
		}

		void AddPacketToQueue(DeusCore::PacketUPtr p_packet);
		void SetGameId(unsigned int value) { 
			m_gameIdLock.lock();
			m_gameId = value; 
			m_gameIdLock.unlock();
		}

	protected:
		// Entry point for the communication thread
		// Recv(), Send() and raise OnMessageRecevied/Disconnected events
		virtual void ThreadSendAndReceive() = 0;

		// Try to take pack in the sending queue
		bool TryTakePacket(DeusCore::PacketUPtr& p_packet);


		///////////////////////////////////////
		//              THREADING            //
		///////////////////////////////////////
		
		int m_id;
		unsigned int m_gameId;

		// Send/Receive thread
		std::thread m_communicationThread;

		// Queue of packets to send
		std::queue<DeusCore::PacketUPtr> m_packetsToSend;

		// mutex on packet's queue
		std::mutex m_packetQueueLock;

		// mutex on gameid
		std::mutex m_gameIdLock;

		///////////////////////////////////////
		//            COMMUNICATION          //
		///////////////////////////////////////

		// buffers used by the thread
		DeusCore::Buffer512 writeBuffer, readBuffer, deserializeBuffer;

		// General buffer 
		std::vector<char> allByteReceivedBuffer;

		// Counter 
		int sentByteCount, readedByteCount;
		
	};
}


