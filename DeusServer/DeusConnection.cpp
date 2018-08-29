#include "DeusConnection.h"
#include "DeusCore/Logger.h"

namespace DeusServer
{
	DeusConnection::DeusConnection(int id)
		: m_id(id)
	{
		m_gameId = 0;
	}

	//---------------------------------------------------------------------------------
	void DeusConnection::AddPacketToQueue(DeusCore::PacketSPtr p_packet)
	{
		if (!p_packet)
			return;

		m_packetQueueLock.lock();
		//DeusCore::Logger::Instance()->Log("UDP Client " + std::to_string(m_id), "Push packet");
		m_packetsToSend.push_back(std::make_pair(0, p_packet));
		m_packetQueueLock.unlock();

		p_packet.reset(); // we don't have packet ownership anymore
	}

	//---------------------------------------------------------------------------------
	void DeusConnection::SetGameId(unsigned int value)
	{
		m_gameIdLock.lock();
		m_gameId = value;
		m_gameIdLock.unlock();
	}

}

