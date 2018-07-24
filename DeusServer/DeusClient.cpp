#include "DeusClient.h"

#include "DeusCore/Logger.h"

#include <string>
#include <iostream>
#include <iomanip>

namespace DeusServer
{
	DeusClient::DeusClient(int id, std::unique_ptr<DeusCore::TcpSocket> communicationSocket)
		: m_tcpConnection(id)
	{
		m_id = id;
		m_tcpConnection.Init(std::move(communicationSocket));
	}

	DeusClient::~DeusClient()
	{		
	}
	
	void DeusClient::SendPacket(DeusCore::PacketUPtr p_packet, bool isTcp)
	{
		if (isTcp)
			m_tcpConnection.AddPacketToQueue(std::move(p_packet));
		else
			throw std::exception("UDP Not implemented");
	}

	void DeusClient::SetConnectionGameId(unsigned int gameId)
	{
		m_tcpConnection.SetGameId(gameId);
	}


}
