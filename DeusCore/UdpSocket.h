#pragma once
#include "CommunicationSocket.h"
namespace DeusCore
{
	class UdpSocket : public CommunicationSocket
	{
	public:
		UdpSocket();
		~UdpSocket();

		void Init(const std::string& ipAdress, const std::string& port, const bool isClient, const bool setNonBlocking);

		virtual bool SendDatas(const char * data, size_t size, int & byteSent) override;
		virtual bool RecvDatas(char * data, size_t size, int & byteRecv) override;
	protected:
		// Informations of the server
		addrinfo* m_clientConnectedInfos = nullptr;
	};
}

