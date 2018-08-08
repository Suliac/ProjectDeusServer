#pragma once
#include "DeusConnection.h"
#include "DeusCore/UdpSocket.h"

namespace DeusServer
{
	class DeusUdpConnection : public DeusConnection
	{
	public:
		DeusUdpConnection(int id);
		~DeusUdpConnection();

		void Init(const std::string& ipAddr, const std::string& port);
		void SetUdpConnectionInitialized() { m_udpConnectionInitialized = true; }
	protected:
		virtual void ThreadSendAndReceive() override;

		virtual bool TryTakePacket(DeusCore::PacketSPtr & p_packet) override;

		const size_t ACK_NUMBER = 3;
	private:
		bool m_udpConnectionInitialized = false;

		// UDP connection handler
		std::unique_ptr<DeusCore::UdpSocket> m_clientUDPSocket;

		std::list<std::pair<long, DeusCore::PacketSPtr>> m_packetsToRequeue;

		std::vector<uint32_t> m_idsAcknoledged;
		uint8_t m_indexForIdsAcknoledged;
	};
}

