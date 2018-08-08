#include "UdpSocket.h"
#include "DeusSocketException.h"
#include "Logger.h"

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

// link with ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")
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

		char clienthost[NI_MAXHOST];  //The clienthost will hold the IP address.
		char clientservice[NI_MAXSERV];
		int theErrorCode = getnameinfo(m_distantInfos->ai_addr, sizeof(*m_distantInfos->ai_addr), clienthost, sizeof(clienthost), clientservice, sizeof(clientservice), NI_NUMERICHOST | NI_NUMERICSERV);

		DeusCore::Logger::Instance()->Log(m_name, "sendto : "+ std::string(clienthost) + ":"+ std::string(clientservice));

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