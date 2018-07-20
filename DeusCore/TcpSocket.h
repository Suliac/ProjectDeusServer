#ifndef _TCPSOCKET
#define _TCPSOCKET

#include "Socket.h"
#include "Buffer.h"
#include "Packet.h"

namespace DeusCore
{
	class TcpSocket : public Socket
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
		bool TCPSend(const Packet& packet, size_t& byteSent);

		// Send buffer to the connected socket
		bool TCPSend(const Buffer512& buffer, size_t& byteSent);

		// Send buffer to the connected socket
		bool TCPSend(const char *data, size_t size, size_t &byteSent);

		// Receive ONLY the first packet (and delete other datas) from the connected socket
		bool TCPRecv(std::unique_ptr<Packet>& p_packetReceived, size_t& byteRecv) ;

		// Receive buffer from the connected socket
		bool TCPRecv(Buffer512& buffer, size_t& byteRecv) ;

		// Receive informations from the connected socket
		bool TCPRecv(char *data, size_t size, size_t &byteRecv) ;
	
		
	};
}
#endif // _TCPSOCKET