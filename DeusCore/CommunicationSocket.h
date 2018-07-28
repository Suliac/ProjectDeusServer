#pragma once
#include "Socket.h"
namespace DeusCore
{
	class CommunicationSocket : public Socket
	{
	public:
		CommunicationSocket(std::string name) : Socket(name) {};

		// Send buffer to the connected socket
		virtual bool Send(const Packet& packet, int& byteSent);

		// Send buffer to the connected socket
		virtual bool Send(const Buffer512& buffer, int& byteSent);

		// Send buffer to the connected socket
		virtual bool SendDatas(const char *data, size_t size, int &byteSent) = 0;

		// Receive ONLY the first packet (and delete other datas) from the connected socket
		virtual bool Recv(std::unique_ptr<Packet>& p_packetReceived, int& byteRecv);

		// Receive buffer from the connected socket
		virtual bool Recv(Buffer512& buffer, int& byteRecv);

		// Receive informations from the connected socket
		virtual bool RecvDatas(char *data, size_t size, int &byteRecv) = 0;
	};
}

