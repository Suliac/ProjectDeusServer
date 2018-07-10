#include "TcpSocket.h"
#include "DeusSocketException.h"

#include <WS2tcpip.h>

namespace DeusNetwork
{
	TcpSocket::TcpSocket() : Socket()
	{
	}

	TcpSocket::~TcpSocket()
	{
		SocketClose();
	}

	void TcpSocket::TCPConnect(const SOCKET socket)
	{
		m_handler = socket;
		m_state = SocketState::SOCKET_READY;
	}

	void TcpSocket::TCPConnect(std::string ipAdress, std::string port)
	{
		try {
			SocketInit(AF_INET, SOCK_STREAM, IPPROTO_TCP, AI_PASSIVE, ipAdress, port);
			SocketCreate();
		}
		catch (DeusSocketException const e)
		{
			throw e;
		}

		if (m_distantInfos == nullptr)
			throw DeusSocketException("Impossible to retrieve connection informations");

		// Connect with the infos we got during the init of the socket
		int result = connect(m_handler, m_distantInfos->ai_addr, (int)m_distantInfos->ai_addrlen);
		if (result == SOCKET_ERROR) {
			closesocket(m_handler);
			m_handler = INVALID_SOCKET;
			throw DeusSocketException("TCP Connection for [" + ipAdress + "@" + port + "] failed");
		}

		// we don't need those informations anymore
		freeaddrinfo(m_distantInfos);

		if (m_handler == INVALID_SOCKET) {
			WSACleanup();
			throw DeusSocketException("Unable to connect to server!");
		}

		m_state = SocketState::SOCKET_READY;
	}

	int TcpSocket::TCPSend(const char* sendbuf, unsigned int datasSize)
	{
		if (m_state != SocketState::SOCKET_READY)
			return SOCKET_ERROR;

		int result = send(m_handler, sendbuf, datasSize, 0);
		if (result == SOCKET_ERROR) {
			std::string message = "send failed with error: "+ std::to_string(SocketGetLastError());
			closesocket(m_handler); 
			throw DeusSocketException(message);
		}

		return result;
	}

	int TcpSocket::TCPRecv(char* datas, unsigned int datasSize)
	{
		int result = recv(m_handler, datas, datasSize, 0); // récupérer les données réçue sur cette socket
		if (result < 0) // données reçues
		{
			std::string message = "recv failed with error: " + std::to_string(SocketGetLastError());
			closesocket(m_handler);
			throw DeusSocketException(message);
		}

		return result;
	}
}


