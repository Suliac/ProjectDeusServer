#include "Socket.h"
#include "DeusSocketException.h"

#include <sstream>
#include <WS2tcpip.h>


namespace DeusNetwork
{
	Socket::Socket()
	{
		m_state = SocketState::SOCKET_NOT_INITIALIZED;
	}

	Socket::~Socket()
	{
	}

	void Socket::SocketInit(short family, short type, IPPROTO protocol, short flags, const std::string& ipAdress, const std::string& port)
	{
		m_distantInfos = nullptr;

		// Initialize Winsock api
		int iResult = WSAStartup(MAKEWORD(2, 2), &m_wsaData);
		if (iResult != 0) {
			throw DeusSocketException("WSAStartup failed: " + std::to_string(iResult));
		}

		// Get/init addr infos
		ZeroMemory(&m_hints, sizeof(m_hints));
		m_hints.ai_family = family;
		m_hints.ai_socktype = type;
		m_hints.ai_protocol = protocol;
		m_hints.ai_flags = flags;

		// Resolve the local address and port to be used by the peer
		iResult = getaddrinfo(ipAdress.c_str(), port.c_str(), &m_hints, &m_distantInfos);
		if (iResult != 0) {
			WSACleanup();
			throw DeusSocketException("Error when retreiving informations for [" + ipAdress + "@" + port + "]. Error " + std::to_string(iResult));
		}

		m_state = SocketState::SOCKET_INITIALIZED;
	}

	void Socket::SocketCreate()
	{
		// Socket creation : to listen client connections
		m_handler = socket(m_distantInfos->ai_family, m_distantInfos->ai_socktype, m_distantInfos->ai_protocol);
		if (m_handler == INVALID_SOCKET)
		{
			int errorNumber = SocketGetLastError();
			freeaddrinfo(m_distantInfos);
			throw DeusSocketException("Error during the socket creation : "+ std::to_string(errorNumber));
		}
		m_state = SocketState::SOCKET_READY;
	}

	void Socket::SocketClose()
	{
		closesocket(m_handler);
		WSACleanup(); 
		m_state = SocketState::SOCKET_CLOSED;
	}

	int Socket::SocketShutdown() const
	{
		return shutdown(m_handler, SD_SEND);
	}

	int Socket::SocketGetLastError() const
	{
		int result = WSAGetLastError();
		WSACleanup();

		return result;
	}
}