#pragma once
#include <queue>
#include <thread>
#include <mutex>
#include <memory>

#include "DeusTcpConnection.h"

namespace DeusServer
{
	class DeusClient
	{
	public:
		DeusClient(std::unique_ptr<DeusNetwork::TcpSocket> communicationSocket);
		~DeusClient();

	private:
		// Do we want to stop the communication with the client
		bool m_cancellationRequested = false;

		DeusTcpConnection m_tcpConnection;

		int m_id;
	};
}

