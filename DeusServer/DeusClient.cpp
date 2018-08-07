#include "DeusClient.h"

#include "DeusCore/Logger.h"

#include <string>
#include <iostream>
#include <iomanip>

namespace DeusServer
{
	DeusClient::DeusClient(Id id, std::unique_ptr<DeusCore::TcpSocket> communicationSocket, const std::string& servAddr, const std::string& servPort)
		: m_tcpConnection(id), m_udpConnection(id)
	{
		m_id = id;

		m_tcpConnection.Init(std::move(communicationSocket));
		m_udpConnection.Init(servAddr, servPort);
	}

	DeusClient::~DeusClient()
	{
	}

	void DeusClient::SendPacket(DeusCore::PacketUPtr p_packet, bool isTcp)
	{
		if (isTcp)
			m_tcpConnection.AddPacketToQueue(std::move(p_packet));
		else
			m_udpConnection.AddPacketToQueue(std::move(p_packet));
	}

	void DeusClient::SetConnectionGameId(Id gameId)
	{
		m_tcpConnection.SetGameId(gameId);
		m_udpConnection.SetGameId(gameId);
	}


}
