#include "stdafx.h"
#include "Socket.h"

namespace DeusNetwork
{
	Socket::Socket()
	{
		m_state = SOCKET_NOT_INITIALIZED;
	}

	Socket::~Socket()
	{
		if (m_distantInfos != nullptr)
			freeaddrinfo(m_distantInfos);
	}

	int Socket::SocketInit(short family, short type, IPPROTO protocol, short flags, const char* ipAdress, const char* port)
	{
		m_distantInfos = nullptr;

		// Initialize Winsock api
		int iResult = WSAStartup(MAKEWORD(2, 2), &m_wsaData);
		if (iResult != 0) {
			printf("WSAStartup failed: %d\n", iResult);
			return SOCKET_ERROR;
		}

		// Get/init addr infos
		ZeroMemory(&m_hints, sizeof(m_hints));
		m_hints.ai_family = family;
		m_hints.ai_socktype = type;
		m_hints.ai_protocol = protocol;
		m_hints.ai_flags = flags;

		// Resolve the local address and port to be used by the peer
		iResult = getaddrinfo(ipAdress, port, &m_hints, &m_distantInfos);
		if (iResult != 0) {
			printf("getaddrinfo failed: %d\n", iResult);
			WSACleanup();
			return SOCKET_ERROR;
		}

		m_state = SOCKET_INITIALIZED;
		return 0;
	}

	int Socket::SocketCreate()
	{
		// Socket creation : to listen client connections
		m_handler = socket(m_distantInfos->ai_family, m_distantInfos->ai_socktype, m_distantInfos->ai_protocol);
		if (m_handler == INVALID_SOCKET)
		{
			printf("Error at socket(): %ld\n", SocketGetLastError());
			freeaddrinfo(m_distantInfos);
			return SOCKET_ERROR;
		}
		m_state = SOCKET_READY;

		return 0;
	}

	void Socket::SocketClose()
	{
		closesocket(m_handler);
		WSACleanup(); 
		m_state = SOCKET_CLOSED;
	}

	int Socket::SocketShutdown()
	{
		return shutdown(m_handler, SD_SEND);
	}

	int Socket::SocketGetLastError()
	{
		int result = WSAGetLastError();
		WSACleanup();

		return result;
	}
}