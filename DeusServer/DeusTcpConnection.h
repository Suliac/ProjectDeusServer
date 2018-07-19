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

		void Init(std::unique_ptr<DeusNetwork::TcpSocket> communicationSocket);
		void Stop() { m_cancellationRequested = true; }

	protected:
		virtual void ThreadSendAndReceive() override;
		
		// Do we want to stop the connection ?
		bool m_cancellationRequested = false;

	private:
		// TCP connection handler
		std::unique_ptr<DeusNetwork::TcpSocket> m_clientTCPSocket;

	};
}


