#ifndef _TCPSOCKET
#define _TCPSOCKET

#include "CommunicationSocket.h"

namespace DeusCore
{
	class TcpSocket : public CommunicationSocket
	{
	public:
		TcpSocket();
		~TcpSocket();

		// Methods

		// Init TCP communication from an ip adress & a port
		// return true if connection succeded otherwise return false
		bool TCPConnect(const std::string& ipAdress, const std::string& port, const bool setNonBlocking = false);

		// Init TCP communication from an existing SOCKET
		void TCPConnect(const SOCKET socket, const bool setNonBlocking = false);
		
		// Send buffer to the connected socket
		virtual bool SendDatas(const char *data, size_t size, int &byteSent)override;

		// Receive informations from the connected socket
		virtual bool RecvDatas(char *data, size_t size, int &byteRecv)override;


	};
}
#endif // _TCPSOCKET