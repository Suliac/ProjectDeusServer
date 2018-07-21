#pragma once

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

#include <WinSock2.h>
#include <iostream>
#include <string>

namespace DeusCore
{
#define DEFAULT_SOCKETSTATE_TIMEOUT 1
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
		Socket(std::string name);
		~Socket();
		
		// Set our socket asynchronous 
		void SetNonBlocking(bool value = true);

		// Close socket (Hard)
		void SocketClose();

		// Close socket (Soft)
		int SocketShutdown() const;

		// Is there any pending data on the socket?
		bool DataAvailable();
	protected:

		///////////////
		//Methods

		// Init the informations needed for socket creation
		void SocketInit(short family, short type, IPPROTO protocol, short flags, const std::string& ipAdress, const std::string& port);

		// Real Socket creation
		void SocketCreate();
		
		bool CheckSocketStates(bool isWritable, bool isReadable, unsigned int timeoutSecond = DEFAULT_SOCKETSTATE_TIMEOUT, unsigned int timeoutMicroseconds = 0);
		
		///////////////
		// Attributes

		// Socket which permit communication
		SOCKET m_handler = INVALID_SOCKET;

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

		// Manage Wsa lifetime
		bool m_isWsaAlive = false;

		// Debug name
		std::string m_name;

		// Check if socket is writable, readable or blocked
		SocketStateFlag CheckSocketStates(unsigned int timeoutSecond, unsigned int timeoutMicroseconds = 0);
	};
}

