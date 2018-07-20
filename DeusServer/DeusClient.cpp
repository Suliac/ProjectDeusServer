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

		//////////////////////////////////////////////////
		// NB : here for debug
		// create deleguate functions
		DeusEventDeleguate messageTcpReceivedDeleguate = fastdelegate::MakeDelegate(this, &DeusClient::ManageTcpMessageReceivedEvent);
		DeusEventDeleguate disconnectTcpDeleguate = fastdelegate::MakeDelegate(this, &DeusClient::ManageTcpDisconnectedEvent);

		// register deleguates with connection
		m_tcpConnection.AddListener(messageTcpReceivedDeleguate, DeusConnection::DeusConnectionEventsType::OnMessageReceived);
		m_tcpConnection.AddListener(disconnectTcpDeleguate, DeusConnection::DeusConnectionEventsType::Disconnected);
		///////////////////////////////////////////////////

		m_tcpConnection.Init(std::move(communicationSocket));
	}

	DeusClient::~DeusClient()
	{
		// create deleguate functions to retrieve old values !
		// so we don't have to cache the deleguate
		DeusEventDeleguate messageTcpReceivedDeleguate = fastdelegate::MakeDelegate(this, &DeusClient::ManageTcpMessageReceivedEvent);
		DeusEventDeleguate disconnectTcpDeleguate = fastdelegate::MakeDelegate(this, &DeusClient::ManageTcpDisconnectedEvent);

		// remove deleguates
		m_tcpConnection.RemoveListener(messageTcpReceivedDeleguate, DeusConnection::DeusConnectionEventsType::OnMessageReceived);
		m_tcpConnection.RemoveListener(disconnectTcpDeleguate, DeusConnection::DeusConnectionEventsType::Disconnected);


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

	void DeusClient::ManageTcpMessageReceivedEvent(int id, DeusCore::PacketSPtr p_packet)
	{
		DeusCore::Logger::Instance()->Log("Client " + std::to_string(id), "Received TCP message of type : " + std::to_string(p_packet->GetID()));
		//std::cout << "[Client " << std::setw(3)<< id << "] Received TCP message of type : " << p_packet->GetID() << std::endl;
	}

	void DeusClient::ManageTcpDisconnectedEvent(int id, DeusCore::PacketSPtr p_packet)
	{
		DeusCore::Logger::Instance()->Log("Client " + std::to_string(id), "TCP Disconnected");
		//std::cout << "[Client " << std::setw(3) << id << "] TCP Disconnected" << std::endl;
	}

}
