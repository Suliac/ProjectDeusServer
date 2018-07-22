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
	using DeusEventDeleguate = fastdelegate::FastDelegate2<int, DeusCore::PacketSPtr>;
	using DeusEventDeleguateVector = std::vector<DeusEventDeleguate>;


#define DEFAULT_DEUSCLIENT_BUFFER_SIZE 2048
	class DeusConnection
	{
	public:
		enum class DeusConnectionEventsType {
			OnMessageReceived = 0x0,
			Disconnected = 0x1
		};
		using DeusConnectionEventDeleguateMap = std::map<DeusConnectionEventsType, DeusEventDeleguateVector>;
		
		DeusConnection(int id) { m_id = id; }

		void AddPacketToQueue(DeusCore::PacketUPtr p_packet);

		bool AddListener(const DeusEventDeleguate& eventDeleguate, const DeusConnectionEventsType type);
		bool RemoveListener(const DeusEventDeleguate& eventDeleguate, const DeusConnectionEventsType type);

	protected:
		// Entry point for the communication thread
		// Recv(), Send() and raise OnMessageRecevied/Disconnected events
		virtual void ThreadSendAndReceive() = 0;

		bool TriggerEvent(const DeusCore::PacketSPtr& p_packet, const DeusConnectionEventsType type);

		// Try to take pack in the sending queue
		bool TryTakePacket(DeusCore::PacketUPtr& p_packet);


		///////////////////////////////////////
		//              THREADING            //
		///////////////////////////////////////
		
		int m_id;

		// Send/Receive thread
		std::thread m_communicationThread;

		// Queue of packets to send
		std::queue<DeusCore::PacketUPtr> m_packetsToSend;

		// mutex on packet's queue
		std::mutex m_packetQueueLock;
		

		///////////////////////////////////////
		//            COMMUNICATION          //
		///////////////////////////////////////

		// buffers used by the thread
		DeusCore::Buffer512 writeBuffer, readBuffer, deserializeBuffer;

		// General buffer 
		std::vector<char> allByteReceivedBuffer;

		// Counter 
		int sentByteCount, readedByteCount;

		///////////////////////////////////////
		//               Events              //
		///////////////////////////////////////

		// map of 'DeusConnectionEventsType' paired with list of deleguate
		DeusConnectionEventDeleguateMap m_eventListeners;

	};
}


