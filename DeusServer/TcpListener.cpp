#include "stdafx.h"
#include "TcpListener.h"

namespace DeusNetwork
{
	TcpListener::TcpListener() : Socket()
	{
	}


	TcpListener::~TcpListener()
	{
	}

	int TcpListener::Init(const char* ipAdress, const char * port)
	{
		if (SocketInit(AF_INET, SOCK_STREAM, IPPROTO_TCP, AI_PASSIVE, ipAdress, port) == SOCKET_ERROR)
			return SOCKET_ERROR;
		if (SocketCreate() < 0)
			return SOCKET_ERROR;

		if (m_distantInfos == nullptr)
			return SOCKET_ERROR;

		return 0;
	}

	int TcpListener::Start()
	{
		// Setup the TCP listening socket
		int result = bind(m_handler, m_distantInfos->ai_addr, (int)m_distantInfos->ai_addrlen);
		if (result == SOCKET_ERROR) {
			printf("bind failed with error: %d\n", WSAGetLastError());
			freeaddrinfo(m_distantInfos);
			closesocket(m_handler);
			WSACleanup();
			return SOCKET_ERROR;
		}

		freeaddrinfo(m_distantInfos);

		result = listen(m_handler, SOMAXCONN);
		if (result == SOCKET_ERROR) {
			printf("listen failed with error: %d\n", WSAGetLastError());
			closesocket(m_handler);
			WSACleanup();
			return SOCKET_ERROR;
		}

		m_state = SOCKET_READY;
		return 0;
	}

	const SOCKET TcpListener::Accept()
	{
		SOCKET clientSocket = accept(m_handler, NULL, NULL);
		if (clientSocket == INVALID_SOCKET) {
			printf("accept failed with error: %d\n", WSAGetLastError());
			closesocket(m_handler);
			WSACleanup();
			return SOCKET_ERROR;
		}

		return clientSocket;
	}
	

}
