#include "TcpListener.h"
#include "DeusSocketException.h"

#include <WS2tcpip.h>

namespace DeusNetwork
{
	TcpListener::TcpListener() : Socket("TcpListener")
	{
	}


	TcpListener::~TcpListener()
	{
	}

	void TcpListener::Init(const std::string& ipAdress, const std::string& port, const bool setNonBlocking)
	{
		SocketInit(AF_INET, SOCK_STREAM, IPPROTO_TCP, AI_PASSIVE, ipAdress, port);
		SocketCreate();

		if (setNonBlocking)
			SetNonBlocking();

		if (m_distantInfos == nullptr)
		{
			SocketClose();
			throw DeusSocketException("Impossible to retrieve connection informations");
		}
	}

	void TcpListener::Start()
	{
		// Setup the TCP listening socket
		int result = bind(m_handler, m_distantInfos->ai_addr, (int)m_distantInfos->ai_addrlen);
		if (result == SOCKET_ERROR) {
			std::string message = "Socket binding failed with error: " + std::to_string(WSAGetLastError());
			freeaddrinfo(m_distantInfos);
			SocketClose();
			throw DeusSocketException(message);
		}

		freeaddrinfo(m_distantInfos);

		result = listen(m_handler, SOMAXCONN);
		if (result == SOCKET_ERROR) {
			std::string message = "Socket listening failed with error: " + std::to_string(WSAGetLastError());
			SocketClose();
			throw DeusSocketException(message);
		}

	}

	bool TcpListener::Accept(TcpSocket& socket)
	{
		if (!CheckSocketStates(false, true))
			return false;

		SOCKET clientSocket = accept(m_handler, NULL, NULL);
		if (clientSocket == INVALID_SOCKET)
		{
			int error =  WSAGetLastError(); 
			if (error != WSAEWOULDBLOCK) { // EWOULDBLOCK isn't a 'real' error
				std::string message = "Accept connection failed with error: " + std::to_string(error);
				SocketClose();
				throw DeusSocketException(message);
			}

			return false;
		}

		socket.TCPConnect(clientSocket, m_isNonBlocking);
		return true;
	}


}
