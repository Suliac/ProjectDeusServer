#include "DeusClient.h"
#include <string>
#include <iostream>

namespace DeusServer
{
	DeusClient::DeusClient(std::unique_ptr<DeusNetwork::TcpSocket> communicationSocket)
	{
		m_tcpConnection.Init(std::move(communicationSocket));
	}
	
	DeusClient::~DeusClient()
	{
	}


}
