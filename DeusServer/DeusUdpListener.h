#pragma once
#include "DeusConnection.h"
#include "DeusCore/UdpSocket.h"
#include "DeusCore/PacketSendUDP.h"
#include "DeusListeningUdpConnection.h"

#include <list>
namespace DeusServer
{
	using PacketUDPSPtr = std::shared_ptr<DeusCore::PacketSendUDP>;
	using UDPPackets = std::list<std::pair<long, PacketUDPSPtr>>;

	class DeusUdpListener
	{
	public:
		/////////////////////////
		//     SINGLETON	   //
		/////////////////////////
		static DeusUdpListener* Instance();
		static void Free();
		
		void AddPacketToQueue(PacketUDPSPtr p_packet);
		void NewClient(std::shared_ptr<DeusListeningUdpConnection> p_udpConnection, std::shared_ptr<addrinfo> p_addr);
		void Stop() { m_cancellationRequested = true; }

	private:
		/////////////////////////
		//     SINGLETON	   //
		/////////////////////////

		DeusUdpListener();
		~DeusUdpListener();

		static DeusUdpListener* p_instance;
		static std::mutex m_instanceLock;
		
		/////////////////////////
		//		  THREAD	   //
		/////////////////////////

		void ThreadSendAndReceive();
		bool TryTakePacket(PacketUDPSPtr& p_packet);

		// Send/Receive thread
		std::thread m_communicationThread;

		// Queue of packets to send, paquets are saved with their last sent date
		// long represent the TickCount
		std::list<std::pair<long, PacketUDPSPtr>> m_packetsToSend;

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

		// mutex on packet's queue
		std::mutex m_packetQueueLock;

		///////////////////////////////////////
		//					UDP				 //
		///////////////////////////////////////

		const size_t ACK_NUMBER = 3;

		// UDP connection handler
		std::unique_ptr<DeusCore::UdpSocket> m_clientUDPSocket;

		UDPPackets m_packetsToRequeue;

		std::vector<uint32_t> m_idsAcknoledged;
		uint8_t m_indexForIdsAcknoledged;

		///////////////////////////////////////
		//		  INTERN COMMUNICATION		 //
		///////////////////////////////////////
		std::shared_ptr<DeusListeningUdpConnection> FindClientWithAddr(std::shared_ptr<addrinfo> p_addr);
		std::list<std::pair<std::shared_ptr<addrinfo>, std::shared_ptr<DeusListeningUdpConnection>>> m_clientWithAdress;
		std::mutex clientsLock;
	};
}

