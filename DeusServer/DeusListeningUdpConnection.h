#pragma once
#include "DeusConnection.h"
#include <WS2tcpip.h>

namespace DeusServer
{
	class DeusListeningUdpConnection : public DeusConnection
	{
	public:
		DeusListeningUdpConnection(Id identifier);
		~DeusListeningUdpConnection();

		void Init(const std::string& addr, const std::string& port);
		void RecvPacket(DeusCore::PacketSPtr p_packet);
	private:
		virtual void ThreadSendAndReceive() override;
		virtual bool TryTakePacket(DeusCore::PacketSPtr & p_packet) override;

		std::shared_ptr<addrinfo> p_clientInfo;
		bool m_isInit;
	};

}
