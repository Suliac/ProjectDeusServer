#pragma once
#include "DeusCore/TcpSocket.h"
#include "DeusConnection.h"

namespace DeusServer
{
	class DeusTcpConnection : public DeusConnection
	{
	public:
		DeusTcpConnection(int id);
		~DeusTcpConnection();

		void Init(std::unique_ptr<DeusCore::TcpSocket> communicationSocket);

		
	protected:
		virtual void ThreadSendAndReceive() override;

		virtual bool TryTakePacket(DeusCore::PacketSPtr & p_packet) override;

	private:
		// TCP connection handler
		std::unique_ptr<DeusCore::TcpSocket> m_clientTCPSocket;
	};
}


