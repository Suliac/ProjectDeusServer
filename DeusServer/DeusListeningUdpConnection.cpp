#include "DeusListeningUdpConnection.h"

#include "DeusCore/EventManagerHandler.h"
#include "DeusCore/DeusSocketException.h"
namespace DeusServer
{
	void DeleteAddr(addrinfo* addr) {
		if (addr != nullptr)
			freeaddrinfo(addr);
	}

	DeusListeningUdpConnection::DeusListeningUdpConnection(Id identifier)
		: DeusConnection(identifier), p_clientInfo(nullptr, DeleteAddr)
	{
	}


	DeusListeningUdpConnection::~DeusListeningUdpConnection()
	{
	}

	/*void DeusListeningUdpConnection::Init(const std::string & addr, const std::string & port)
	{
		// 1 - Save client informations
		addrinfo hints;

		ZeroMemory(&p_clientInfo, sizeof(p_clientInfo));
		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_DGRAM;
		hints.ai_protocol = IPPROTO_UDP;
		hints.ai_flags = AI_PASSIVE;

		// Resolve the local address and port to be used
		addrinfo *temp;
		int iResult = getaddrinfo(addr.c_str(), port.c_str(), &hints, &temp);
		if (iResult != 0) {
			throw DeusCore::DeusSocketException("Error when retreiving informations for [" + addr + "@" + port + "]. Error " + std::to_string(iResult));
		}
		p_clientInfo.reset(temp);
	}*/

	void DeusListeningUdpConnection::RecvPacket(DeusCore::PacketSPtr p_packet)
	{
		DeusCore::EventManagerHandler::Instance()->QueueEvent(m_gameId, m_id, p_packet);
	}

	void DeusListeningUdpConnection::ThreadSendAndReceive()
	{
	}
	bool DeusListeningUdpConnection::TryTakePacket(DeusCore::PacketSPtr & p_packet)
	{
		return false;
	}
}
