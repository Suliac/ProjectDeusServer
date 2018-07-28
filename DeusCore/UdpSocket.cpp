#include "UdpSocket.h"
#include "DeusSocketException.h"

#include <assert.h>
namespace DeusCore
{
	UdpSocket::UdpSocket() : CommunicationSocket("UdpSocket")
	{
	}

	//---------------------------------------------------------------------------------
	UdpSocket::~UdpSocket()
	{
	}

	//---------------------------------------------------------------------------------
	void UdpSocket::Init(const std::string & ipAdress, const std::string & port, const bool isClient, const bool setNonBlocking)
	{
		assert(ipAdress != "");
		assert(port != "" );

		SocketInit(AF_INET, SOCK_DGRAM, IPPROTO_UDP, AI_PASSIVE, ipAdress, port);
		SocketCreate();

		if (setNonBlocking)
			SetNonBlocking();

		if (!isClient)
			SocketBind();
	}
		
	//---------------------------------------------------------------------------------
	// Send char* buffer of datas
	//---------------------------------------------------------------------------------
	bool UdpSocket::SendDatas(const char * data, size_t size, int & byteSent)
	{
		assert(m_distantInfos != nullptr);

		if (!CheckSocketStates(true, false)) // Check if socket is writable
			return false;

		byteSent = sendto(m_handler, data, size, 0, m_distantInfos->ai_addr, m_distantInfos->ai_addrlen);
		if (byteSent == SOCKET_ERROR) {
			std::string message = "UDP send failed with error: " + std::to_string(WSAGetLastError());
			SocketClose();
			throw DeusSocketException(message);
		}

		return true;
	}

	//---------------------------------------------------------------------------------
	// Receive char* buffer of datas 
	//---------------------------------------------------------------------------------
	bool UdpSocket::RecvDatas(char * data, size_t size, int & byteRecv)
	{
		if (!CheckSocketStates(false, true)) // Check if socket is readable
			return false;

		int slen = (int)m_distantInfos->ai_addrlen;
		byteRecv = recvfrom(m_handler, data, size, 0, m_distantInfos->ai_addr, &slen); // read socket's datas
		if (byteRecv < 0) // if there error, throw a DeusSocketException
		{
			std::string message = "recv failed with error: " + std::to_string(WSAGetLastError());
			SocketClose();
			throw DeusSocketException(message);
		}

		return true;
	}
}