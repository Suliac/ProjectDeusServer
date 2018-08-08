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
	void DeusConnection::AddPacketToQueue(DeusCore::PacketUPtr p_packet)
	{
		if (!p_packet)
			return;
		DeusCore::PacketSPtr p_sPacket = std::move(p_packet);

		m_packetQueueLock.lock();
		DeusCore::Logger::Instance()->Log("UDP Client " + std::to_string(m_id), "Push packet");
		m_packetsToSend.push_back(std::make_pair(0, p_sPacket));
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

	//---------------------------------------------------------------------------------
	/*bool DeusConnection::TryTakePacket(DeusCore::PacketSPtr & p_packet)
	{
		bool popedElement = false;

		// don't block the execution :
		// add packet is more important than try to take them
		if (m_packetQueueLock.try_lock()) 
		{
			if (m_packetsToSend.size() > 0)
			{
				p_packet = std::move(m_packetsToSend.front().second);
				m_packetsToSend.pop();

				popedElement = true;
			}
			m_packetQueueLock.unlock();
		}

		return popedElement;
	}*/
}

