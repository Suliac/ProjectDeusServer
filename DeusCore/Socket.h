#pragma once

#include <WinSock2.h>
#include <iostream>
#include <string>

namespace DeusNetwork
{
	enum class SocketState {
		SOCKET_NOT_INITIALIZED,
		SOCKET_INITIALIZED,
		SOCKET_READY,
		SOCKET_NOT_READY,
		SOCKET_CLOSED
	};

	class Socket
	{
	public:
		// Close socket (Hard)
		void SocketClose();

		// Close socket (Soft)
		int SocketShutdown();

	protected:
		Socket();
		~Socket();

		//Methods

		// Init the informations needed for socket creation
		void SocketInit(short family, short type, IPPROTO protocol, short flags, std::string ipAdress, std::string port);

		// Real Socket creation
		void SocketCreate();

		// Attributes

		// Get last error from wsa datas
		int SocketGetLastError();

		// Socket which permit communication
		SOCKET m_handler;

		// Pre-Informations to find informations 
		addrinfo m_hints;

		// Informations of the peer 
		addrinfo* m_distantInfos = nullptr;

		// Current state of our socket
		SocketState m_state;
	private:
		// Data we need for winsocks
		WSADATA m_wsaData;
	};
}

