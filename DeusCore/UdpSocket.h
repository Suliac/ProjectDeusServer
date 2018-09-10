#pragma once
#include "CommunicationSocket.h"
#include "PacketSendUDP.h"
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
		
		bool SendTo(const PacketSendUDP& packet, int & byteSent);
		bool SendTo(const Buffer512 & buffer, int & byteSent, std::shared_ptr<const addrinfo> addr);
		bool SendDatasTo(const char * data, size_t size, int & byteSent, std::shared_ptr<const addrinfo> addr);
		
		bool RecvFrom(Buffer512& buffer, int& byteRecv, std::shared_ptr<addrinfo> addr);
		bool RecvDatasFrom(char * data, size_t size, int & byteRecv, std::shared_ptr<addrinfo> addr);
	protected:
		// Informations of the server
		addrinfo* m_clientConnectedInfos = nullptr;
	};
}

