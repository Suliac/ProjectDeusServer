#ifndef _TCPSOCKET
#define _TCPSOCKET

#include "Socket.h"
#include "Buffer.h"

namespace DeusNetwork
{
	class TcpSocket : public Socket
	{
	public:
		TcpSocket();
		~TcpSocket();

		// Methods

		// Init TCP communication from an ip adress & a port
		void TCPConnect(std::string ipAdress, std::string port);

		// Init TCP communication from an existing SOCKET
		void TCPConnect(const SOCKET socket);

		// Send buffer to the connected socket
		int TCPSend(const Buffer512& buffer)const ;

		// Receive informations from the connected socket
		int TCPRecv(Buffer512& buffer) const;
	};
}
#endif // _TCPSOCKET