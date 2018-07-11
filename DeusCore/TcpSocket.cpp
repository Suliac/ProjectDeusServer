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

	void TcpSocket::TCPConnect(const std::string& ipAdress, const std::string& port)
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

	int TcpSocket::TCPSend(const Buffer512& buffer) const
	{
		if (m_state != SocketState::SOCKET_READY)
			return SOCKET_ERROR;

		int nbrBytesSent = send(m_handler, reinterpret_cast<const char*>(buffer.data), buffer.index, 0);
		if (nbrBytesSent == SOCKET_ERROR) {
			std::string message = "send failed with error: "+ std::to_string(SocketGetLastError());
			closesocket(m_handler); 
			throw DeusSocketException(message);
		}

		return nbrBytesSent;
	}

	int TcpSocket::TCPRecv(Buffer512& buffer) const
	{
		int recvSize = recv(m_handler, (char*)buffer.data, buffer.size, 0); // r�cup�rer les donn�es r��ue sur cette socket
		if (recvSize < 0) // donn�es re�ues
		{
			std::string message = "recv failed with error: " + std::to_string(SocketGetLastError());
			closesocket(m_handler);
			throw DeusSocketException(message);
		}

		buffer.index = 0;
		return recvSize;
	}
}


