#ifndef _SOCKET
#define _SOCKET

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdio.h>

namespace DeusNetwork
{
	enum SocketState {
		SOCKET_NOT_INITIALIZED,
		SOCKET_INITIALIZED,
		SOCKET_READY,
		SOCKET_NOT_READY,
		SOCKET_CLOSED
	};

	class Socket
	{
	public:
		/// Close socket (Hard)
		void SocketClose();

		/// Close socket (Soft)
		int SocketShutdown();

	protected:
		Socket();
		~Socket();

		//Methods
		/// Init the informations needed for socket creation
		int SocketInit(short family, short type, IPPROTO protocol, short flags, const char* ipAdress, const char* port);

		/// Real Socket creation
		int SocketCreate();

		// Attributes
		/// Get last error from wsa datas
		int SocketGetLastError();

		/// Socket which permit communication
		SOCKET m_handler;

		/// Pre-Informations to find informations 
		addrinfo m_hints;

		/// Informations of the peer 
		addrinfo* m_distantInfos = nullptr;

		/// Current state of our socket
		SocketState m_state;
	private:
		/// Data we need for winsocks
		WSADATA m_wsaData;
	};
}
#endif // _SOCKET

