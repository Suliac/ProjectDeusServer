#pragma once

#include <WinSock2.h>
#include <iostream>
#include <string>

namespace DeusNetwork
{
#define DEFAULT_SOCKETSTATE_TIMEOUT 3
	/////////////////////
	// Socket States
	enum class SocketStateFlag {
		SOCKET_READY = 0,
		SOCKET_WRITE_NOT_READY = 1,
		SOCKET_READ_NOT_READY = 2,
		SOCKET_NOT_READY = 4,
		SOCKET_TIMEOUT = 8
	};

	inline SocketStateFlag operator|(SocketStateFlag a, SocketStateFlag b)
	{
		return static_cast<SocketStateFlag>(static_cast<int>(a) | static_cast<int>(b));
	}

	inline bool operator&(SocketStateFlag a, SocketStateFlag b)
	{
		return static_cast<int>(a) & static_cast<int>(b);
	}

	/////////////////////
	// Socket
	class Socket
	{
	public:
		void SetNonBlocking(bool value = true);

		// Close socket (Hard)
		void SocketClose() const;

		// Close socket (Soft)
		int SocketShutdown() const;

	protected:
		Socket();
		~Socket();

		///////////////
		//Methods

		// Init the informations needed for socket creation
		void SocketInit(short family, short type, IPPROTO protocol, short flags, const std::string& ipAdress, const std::string& port);

		// Real Socket creation
		void SocketCreate();
		
		// Get last error from wsa datas
		int SocketGetLastError() const;

		bool CheckSocketStates(bool isWritable, bool isReadable, unsigned int timeoutSecond = DEFAULT_SOCKETSTATE_TIMEOUT, unsigned int timeoutMicroseconds = 0) const;
		
		///////////////
		// Attributes

		// Socket which permit communication
		SOCKET m_handler;

		// Pre-Informations to find informations 
		addrinfo m_hints;

		// Informations of the peer 
		addrinfo* m_distantInfos = nullptr;

		// Current state of our socket
		//SocketState m_state;

		bool m_isNonBlocking = false;
	private:
		// Data we need for winsocks
		WSADATA m_wsaData;

		SocketStateFlag CheckSocketStates(unsigned int timeoutSecond, unsigned int timeoutMicroseconds = 0) const;
	};
}

