#pragma once
#include <thread>
#include <list>
#include <mutex>
#include <map>

#include "DeusCore/Packet.h"
#include "DeusCore/Socket.h"
#include "DeusCore/FastDelegate.h"

namespace DeusServer
{
	// FastDelegate2<connection id (int), shared_ptr on a packet>
	using DeusClientEventDeleguate = fastdelegate::FastDelegate2<int, DeusCore::PacketSPtr>;
	using DeusClientEventDeleguateVector = std::vector<DeusClientEventDeleguate>;

	// Timout before retry send or receive
	const unsigned int TIMEOUT_US = 1000;
	const unsigned int PACKET_DELAY_CHECK_ACK_MS = 100;
	const unsigned int DECAL_PORT_UDP = 10000;

#define DEFAULT_DEUSCLIENT_BUFFER_SIZE 2048
	class DeusConnection
	{
	public:

		DeusConnection(int id);

		virtual void AddPacketToQueue(DeusCore::PacketSPtr p_packet);

		void SetGameId(unsigned int value);

		void Stop() { m_cancellationRequested = true; }


	protected:
		// Entry point for the communication thread
		// Recv(), Send() and raise OnMessageRecevied/Disconnected events
		virtual void ThreadSendAndReceive() = 0;

		// Try to take pack in the sending queue
		virtual bool TryTakePacket(DeusCore::PacketSPtr& p_packet) = 0;

		///////////////////////////////////////
		//              THREADING            //
		///////////////////////////////////////

		int m_id;
		unsigned int m_gameId;

		// Send/Receive thread
		std::thread m_communicationThread;

		// Queue of packets to send, paquets are saved with their last sent date
		// long represent the TickCount
		std::list<std::pair<long, DeusCore::PacketSPtr>> m_packetsToSend;

		// mutex on packet's queue
		std::recursive_mutex m_packetQueueLock;

		// mutex on gameid
		std::mutex m_gameIdLock;

		///////////////////////////////////////
		//            COMMUNICATION          //
		///////////////////////////////////////

		// buffers used by the thread
		DeusCore::Buffer512 readBuffer, deserializeBuffer;

		// General buffer 
		std::vector<char> allByteReceivedBuffer;

		// Counter 
		int sentByteCount, readedByteCount;
		
		// Do we want to stop the connection ?
		bool m_cancellationRequested = false;

	};

}


