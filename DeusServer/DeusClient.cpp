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

	bool DeusClient::AddListener(const DeusEventDeleguate& eventDeleguate, const DeusClientEventsType type)
	{
		switch (type)
		{
		case DeusClientEventsType::OnTCPMessageReceived:
			return m_tcpConnection.AddListener(eventDeleguate, DeusConnection::DeusConnectionEventsType::OnMessageReceived);

		case DeusClientEventsType::OnTCPDisconnect:
			return m_tcpConnection.AddListener(eventDeleguate, DeusConnection::DeusConnectionEventsType::Disconnected);

		case DeusClientEventsType::OnUDPMessageReceived:
			// TODO when implement UDP connections
			break;

		case DeusClientEventsType::OnUDPDisconnect:
			// TODO when implement UDP connections
			break;
		default:
			break;
		}
		
		return false;
	}

	bool DeusClient::RemoveListener(const DeusEventDeleguate& eventDeleguate, const DeusClientEventsType type)
	{
		switch (type)
		{
		case DeusClientEventsType::OnTCPMessageReceived:
			return m_tcpConnection.RemoveListener(eventDeleguate, DeusConnection::DeusConnectionEventsType::OnMessageReceived);

		case DeusClientEventsType::OnTCPDisconnect:
			return m_tcpConnection.RemoveListener(eventDeleguate, DeusConnection::DeusConnectionEventsType::Disconnected);

		case DeusClientEventsType::OnUDPMessageReceived:
			// TODO when implement UDP connections
			break;

		case DeusClientEventsType::OnUDPDisconnect:
			// TODO when implement UDP connections
			break;
		default:
			break;
		}

		return false;
	}


}
