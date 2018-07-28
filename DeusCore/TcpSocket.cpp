#include "TcpSocket.h"
#include "Logger.h"
#include "DeusSocketException.h"

#include <WS2tcpip.h>

namespace DeusCore
{
	TcpSocket::TcpSocket() : CommunicationSocket("TcpSocket")
	{
	}

	//---------------------------------------------------------------------------------
	TcpSocket::~TcpSocket()
	{
	}

	//---------------------------------------------------------------------------------
	void TcpSocket::TCPConnect(const SOCKET socket, const bool setNonBlocking)
	{
		assert(socket != INVALID_SOCKET);

		m_handler = socket;
		// We can set our socket non blocking
		if (setNonBlocking)
			SetNonBlocking();
	}

	//---------------------------------------------------------------------------------
	bool TcpSocket::TCPConnect(const std::string& ipAdress, const std::string& port, const bool setNonBlocking)
	{
		// 1 - Init socket : get ipadress etc
		SocketInit(AF_INET, SOCK_STREAM, IPPROTO_TCP, AI_PASSIVE, ipAdress, port);

		// 2 - Create the socket
		SocketCreate();
		
		if (m_distantInfos == nullptr)
		{
			SocketClose();
			throw DeusSocketException("Impossible to retrieve connection informations");
		}
		
		// 3 - Connect (with the infos we got during the init of the socket)
		int result = connect(m_handler, m_distantInfos->ai_addr, (int)m_distantInfos->ai_addrlen);
		if (result == SOCKET_ERROR) {
			SocketClose();
			throw DeusSocketException("TCP Connection for [" + ipAdress + "@" + port + "] failed");
		}

		// We can set our socket non blocking when connection succeed
		if (setNonBlocking)
			SetNonBlocking();		

		// we don't need those informations anymore
		FreeInfos();

		if (m_handler == INVALID_SOCKET) {
			SocketClose();
			throw DeusSocketException("Unable to connect to server!");
		}

		return true;
	}

	
	//---------------------------------------------------------------------------------
	// Send char* buffer of datas
	//---------------------------------------------------------------------------------
	bool TcpSocket::SendDatas(const char * data, size_t size, int& byteSent)
	{
		if (!CheckSocketStates(true, false)) // Check if socket is writable
			return false;

		byteSent = send(m_handler, data, size, 0);
		if (byteSent == SOCKET_ERROR) {
			std::string message = "send failed with error: " + std::to_string(WSAGetLastError());
			SocketClose();
			throw DeusSocketException(message);
		}

		return true;
	}


	//---------------------------------------------------------------------------------
	// Receive char* buffer of datas 
	//---------------------------------------------------------------------------------
	bool TcpSocket::RecvDatas(char * data, size_t size, int & byteRecv)
	{
		if (!CheckSocketStates(false, true)) // Check if socket is readable
			return false;

		byteRecv = recv(m_handler, data, size, 0); // read socket's datas
		if (byteRecv < 0) // if there error, throw a DeusSocketException
		{
			std::string message = "recv failed with error: " + std::to_string(WSAGetLastError());
			SocketClose();
			throw DeusSocketException(message);
		}

		return true;
	}

}


