#ifndef _TCPSOCKET
#define _TCPSOCKET

#include "Socket.h"
#include "Buffer.h"
#include "Message.h"

namespace DeusNetwork
{
	class TcpSocket : public Socket
	{
	public:
		TcpSocket();
		~TcpSocket();

		// Methods

		// Init TCP communication from an ip adress & a port
		void TCPConnect(const std::string& ipAdress, const std::string& port);

		// Init TCP communication from an existing SOCKET
		void TCPConnect(const SOCKET socket);
		
		// Send buffer to the connected socket
		void TCPSend(const IMessage& message, size_t& byteSent)const;

		// Send buffer to the connected socket
		void TCPSend(const Buffer512& buffer, size_t& byteSent)const;

		// Send buffer to the connected socket
		void TCPSend(const char *data, size_t size, size_t &byteSent)const;

		// Receive informations from the connected socket
		void TCPRecv(IMessage& message, size_t& byteRecv) const;

		// Receive informations from the connected socket
		void TCPRecv(Buffer512& buffer, size_t& byteRecv) const;

		// Receive informations from the connected socket
		void TCPRecv(char *data, size_t size, size_t &byteRecv) const;
	};
}
#endif // _TCPSOCKET