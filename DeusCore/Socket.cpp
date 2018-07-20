#include "Socket.h"
#include "DeusSocketException.h"

#include <sstream>
#include <WS2tcpip.h>


namespace DeusCore
{
	Socket::Socket(std::string name)
	{
		m_name = "[" + name + " | " + std::to_string(std::rand()) + "]";
		// Initialize WSA
		int iResult = WSAStartup(MAKEWORD(2, 2), &m_wsaData);
		if (iResult != 0) {
			throw DeusSocketException("WSAStartup failed: " + std::to_string(iResult));
		}

		//std::cout << m_name << "WSAStartup" << std::endl;
		m_isWsaAlive = true;
	}

	Socket::~Socket()
	{
		/*if (m_handler != INVALID_SOCKET)
			SocketClose();*/

		if (m_isWsaAlive)
		{
			//std::cout << m_name << "WSACleanup (destructor)" << std::endl;
			WSACleanup();
		}
	}

	void Socket::SocketInit(short family, short type, IPPROTO protocol, short flags, const std::string& ipAdress, const std::string& port)
	{
		m_distantInfos = nullptr;

		// Get/init addr infos
		ZeroMemory(&m_hints, sizeof(m_hints));
		m_hints.ai_family = family;
		m_hints.ai_socktype = type;
		m_hints.ai_protocol = protocol;
		m_hints.ai_flags = flags;

		// Resolve the local address and port to be used by the peer
		int iResult = getaddrinfo(ipAdress.c_str(), port.c_str(), &m_hints, &m_distantInfos);
		if (iResult != 0) {
			WSACleanup();
			throw DeusSocketException("Error when retreiving informations for [" + ipAdress + "@" + port + "]. Error " + std::to_string(iResult));
		}

	}

	void Socket::SocketCreate()
	{
		// Socket creation : to listen client connections
		m_handler = socket(m_distantInfos->ai_family, m_distantInfos->ai_socktype, m_distantInfos->ai_protocol);
		if (m_handler == INVALID_SOCKET)
		{
			int errorNumber = WSAGetLastError();
			SocketClose();
			throw DeusSocketException("Error during the socket creation : " + std::to_string(errorNumber));
		}
		//m_state = SocketState::SOCKET_READY;
	}

	void Socket::SetNonBlocking(bool value)
	{
		m_isNonBlocking = value;
		u_long blockingMode = m_isNonBlocking;

		int iResult = ioctlsocket(m_handler, FIONBIO, &blockingMode);
		if (iResult != NO_ERROR)
		{
			SocketClose();
			throw DeusSocketException("Cannot set socket to blocking mode. ioctlsocket failed with error: " + std::to_string(iResult));
		}

	}

	SocketStateFlag Socket::CheckSocketStates(unsigned int timeoutSecond, unsigned int timeoutMicroseconds) 
	{
		SocketStateFlag state = SocketStateFlag::SOCKET_READY;
		fd_set readFlags, writeFlags, exceptFlags;							// the flag sets to be used
		struct timeval waitTime = { timeoutSecond, timeoutMicroseconds };	// the max wait time for an event
		int selectResult;													// holds return value for select();

		// Init flags
		FD_ZERO(&readFlags);
		FD_ZERO(&writeFlags);
		FD_ZERO(&exceptFlags);
		FD_SET(m_handler, &readFlags);
		FD_SET(m_handler, &writeFlags);
		FD_SET(m_handler, &exceptFlags);

		selectResult = select(m_handler + 1, &readFlags, &writeFlags, &exceptFlags, &waitTime);
		if (selectResult < 0)
		{
			int error = WSAGetLastError();
			SocketClose();
			throw DeusSocketException("Error during select : " + std::to_string(error));
		}

		if (selectResult == 0) // timed out
		{
			state = state | SocketStateFlag::SOCKET_TIMEOUT;
		}
		else
		{
			// The socket is ready to read ?
			if (!FD_ISSET(m_handler, &readFlags)) // If FD_ISSET -> Can read
				state = state | SocketStateFlag::SOCKET_READ_NOT_READY;

			// The socket is ready to write ?
			if (!FD_ISSET(m_handler, &writeFlags)) // If FD_ISSET -> Can write
				state = state | SocketStateFlag::SOCKET_WRITE_NOT_READY;

			// Other blocking errors ?
			if (FD_ISSET(m_handler, &exceptFlags)) // /!\ If FD_ISSET -> error ! 
				state = state | SocketStateFlag::SOCKET_NOT_READY;
		}

		//clear sets
		FD_CLR(m_handler, &readFlags);
		FD_CLR(m_handler, &writeFlags);
		FD_CLR(m_handler, &exceptFlags);

		return state;
	}

	bool Socket::CheckSocketStates(bool isWritable, bool isReadable, unsigned int timeoutSecond, unsigned int timeoutMicroseconds)
	{
		SocketStateFlag states;
		bool canUseSocket = true;

		if (!m_isNonBlocking)
			return true;

		states = CheckSocketStates(DEFAULT_SOCKETSTATE_TIMEOUT);

		if (states & SocketStateFlag::SOCKET_WRITE_NOT_READY && isWritable)
			canUseSocket = false;
		if (states & SocketStateFlag::SOCKET_READ_NOT_READY && isReadable)
			canUseSocket = false;

		if (states & SocketStateFlag::SOCKET_NOT_READY)
			canUseSocket = false;

		if (states & SocketStateFlag::SOCKET_TIMEOUT)
			canUseSocket = false;

		/*system("cls");
		std::cout << "writable : " << ((states & SocketStateFlag::SOCKET_WRITE_NOT_READY && isWritable	)?"oui":"non") << std::endl;
		std::cout << "readable : " << ((states & SocketStateFlag::SOCKET_READ_NOT_READY && isReadable	)?"oui":"non") << std::endl;
		std::cout << "error    : " << ((states & SocketStateFlag::SOCKET_NOT_READY						)?"oui":"non") << std::endl;
		std::cout << "timeout  : " << ((states & SocketStateFlag::SOCKET_TIMEOUT						)?"oui":"non") << std::endl;*/


		return canUseSocket;
	}

	void Socket::SocketClose()
	{
		closesocket(m_handler);
		WSACleanup(); 
		m_isWsaAlive = false;
		//std::cout << m_name << "WSACleanup (SocketClose())" << std::endl;

		m_handler = INVALID_SOCKET;
	}

	int Socket::SocketShutdown() const
	{
		return shutdown(m_handler, SD_SEND);
	}

	bool Socket::DataAvailable() {
		return CheckSocketStates(false, true);
	}
}