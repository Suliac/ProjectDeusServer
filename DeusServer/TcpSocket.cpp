#include "stdafx.h"
#include "TcpSocket.h"

namespace DeusNetwork
{
	TcpSocket::TcpSocket() : Socket()
	{
	}

	TcpSocket::~TcpSocket()
	{
		SocketClose();
	}
	
	int TcpSocket::TCPConnect(const SOCKET socket)
	{
		m_handler = socket;
		m_state = SOCKET_READY;
		return 0;
	}
	
	int TcpSocket::TCPConnect(const char* ipAdress, const char* port)
	{
		if (SocketInit(AF_INET, SOCK_STREAM, IPPROTO_TCP, AI_PASSIVE, ipAdress, port) == SOCKET_ERROR)
			return SOCKET_ERROR;
		if (SocketCreate() == SOCKET_ERROR)
			return SOCKET_ERROR;

		if (m_distantInfos == nullptr)
			return SOCKET_ERROR;

		// Connect with the infos we got during the init of the socket
		int result = connect(m_handler, m_distantInfos->ai_addr, (int)m_distantInfos->ai_addrlen);
		if (result == SOCKET_ERROR) {
			closesocket(m_handler);
			m_handler = INVALID_SOCKET;
			return SOCKET_ERROR;
		}

		// we don't need those informations anymore
		freeaddrinfo(m_distantInfos);

		if (m_handler == INVALID_SOCKET) {
			printf("Unable to connect to server!\n");
			WSACleanup();
			return SOCKET_ERROR;
		}

		m_state = SOCKET_READY;
		return 0;
	}

	int TcpSocket::TCPSend(const char* sendbuf, unsigned int datasSize)
	{
		if (m_state != SOCKET_READY)
			return SOCKET_ERROR;

		int result = send(m_handler, sendbuf, datasSize, 0);
		if (result == SOCKET_ERROR) {
			printf("send failed with error: %d\n", GetLastError());
			closesocket(m_handler);
		}

		return result;
	}

	int TcpSocket::TCPRecv(char* datas, unsigned int datasSize)
	{
		int result = recv(m_handler, datas, datasSize, 0); // récupérer les données réçue sur cette socket
		if (result < 0) // données reçues
		{
			printf("recv failed: %d\n", GetLastError());
			closesocket(m_handler);
			return SOCKET_ERROR;
		}

		return result;
	}
}


