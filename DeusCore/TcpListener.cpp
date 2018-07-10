#include "TcpListener.h"
#include "DeusSocketException.h"

#include <WS2tcpip.h>

namespace DeusNetwork
{
	TcpListener::TcpListener() : Socket()
	{
	}


	TcpListener::~TcpListener()
	{
	}

	void TcpListener::Init(std::string ipAdress, std::string port)
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
	}

	void TcpListener::Start()
	{
		// Setup the TCP listening socket
		int result = bind(m_handler, m_distantInfos->ai_addr, (int)m_distantInfos->ai_addrlen);
		if (result == SOCKET_ERROR) {
			std::string message = "Socket binding failed with error: " + std::to_string(SocketGetLastError());
			freeaddrinfo(m_distantInfos);
			closesocket(m_handler);
			throw DeusSocketException(message);
		}

		freeaddrinfo(m_distantInfos);

		result = listen(m_handler, SOMAXCONN);
		if (result == SOCKET_ERROR) {
			std::string message = "Socket listening failed with error: " + std::to_string(SocketGetLastError());
			closesocket(m_handler);
			throw DeusSocketException(message);
		}

		m_state = SocketState::SOCKET_READY;
	}

	void TcpListener::Accept(TcpSocket& socket)
	{
		SOCKET clientSocket = accept(m_handler, NULL, NULL);
		if (clientSocket == INVALID_SOCKET) {
			std::string message = "Accept connection failed with error: " + std::to_string(SocketGetLastError());
			closesocket(m_handler);
			throw DeusSocketException(message);
		}

		socket.TCPConnect(clientSocket);
	}


}
