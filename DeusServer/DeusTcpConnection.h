#pragma once
#include "DeusCore/TcpSocket.h"
#include "DeusConnection.h"

namespace DeusServer
{
	class DeusTcpConnection : public DeusConnection
	{
	public:
		DeusTcpConnection();
		~DeusTcpConnection();

		void Init(std::unique_ptr<DeusNetwork::TcpSocket> communicationSocket);

	protected:
		virtual void ThreadSendAndReceive() override;

	private:
		// TCP connection handler
		std::unique_ptr<DeusNetwork::TcpSocket> m_clientTCPSocket;

	};
}


