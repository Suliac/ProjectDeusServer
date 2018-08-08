#pragma once

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

#include <WinSock2.h>
#include <iostream>
#include <sstream>
#include <string>

#include "Packet.h"

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
		bool DataAvailable(unsigned int timeoutSecond = DEFAULT_SOCKETSTATE_TIMEOUT, unsigned int timeoutMicroseconds = 0);

		bool IsClosed() const { return m_handler == INVALID_SOCKET; };
		const std::string& GetSocketNumber() const { return std::to_string(m_handler); }

		const std::string& GetIpAddr() const { return m_ipAddr; };
		const std::string& GetIpPort() const { return m_port; };
		uint32_t GetIpNumberPort() const { 
			uint32_t value = 0;
			std::stringstream ssValue(m_port);
			ssValue >> value;
			return value;
		};

		void SetIPEndpoint(const std::string& addr, const std::string& port)
		{
			m_port = port;
			m_ipAddr = addr;
		}
	protected:

		///////////////
		//Methods

		// Init the informations needed for socket creation
		void SocketInit(short family, short type, IPPROTO protocol, short flags, const std::string& ipAdress, const std::string& port);

		// Real Socket creation
		void SocketCreate();

		void SocketBind();

		void FreeInfos();

		bool CheckSocketStates(bool isWritable, bool isReadable, unsigned int timeoutSecond = DEFAULT_SOCKETSTATE_TIMEOUT, unsigned int timeoutMicroseconds = 0);

		// Check if socket is writable, readable or blocked
		SocketStateFlag CheckSocketStates(unsigned int timeoutSecond, unsigned int timeoutMicroseconds = 0);

		///////////////
		// Attributes

		// Socket which permit communication
		SOCKET m_handler = INVALID_SOCKET;

		// Pre-Informations to find informations 
		addrinfo m_hints;

		// Informations of the server
		addrinfo* m_distantInfos = nullptr;

		std::string m_ipAddr;
		std::string m_port;

		bool m_isNonBlocking = false;

		// Debug name
		std::string m_name;
	private:
		// Data we need for winsocks
		WSADATA m_wsaData;

		// Manage Wsa lifetime
		bool m_isWsaAlive = false;

		bool m_freeInfos = false;
	};
}

