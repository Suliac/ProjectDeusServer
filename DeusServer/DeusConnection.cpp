#include "DeusConnection.h"


namespace DeusServer
{
	void DeusConnection::AddPacketToQueue(DeusCore::PacketUPtr p_packet)
	{
		if (!p_packet)
			return;

		m_packetQueueLock.lock();
		m_packetsToSend.push(std::move(p_packet));
		m_packetQueueLock.unlock();

		p_packet.reset(); // we don't have packet ownership anymore
	}
	
	bool DeusConnection::TryTakePacket(DeusCore::PacketUPtr & p_packet)
	{
		bool popedElement = false;

		// don't block the execution :
		// add packet is more important than try to take them
		if (m_packetQueueLock.try_lock()) 
		{
			if (m_packetsToSend.size() > 0)
			{
				p_packet = std::move(m_packetsToSend.front());
				m_packetsToSend.pop();

				popedElement = true;
			}
			m_packetQueueLock.unlock();
		}

		return popedElement;
	}
}

