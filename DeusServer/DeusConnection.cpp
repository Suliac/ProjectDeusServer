#include "DeusConnection.h"


namespace DeusServer
{
	void DeusConnection::AddPacketToQueue(DeusCore::PacketUPtr & p_packet)
	{
		if (!p_packet)
			return;

		m_packetQueueLock.lock();
		m_packetsToSend.push(std::move(p_packet));
		m_packetQueueLock.unlock();

		p_packet.reset(); // we don't have packet ownership anymore
	}

	bool DeusConnection::AddListener(const DeusEventDeleguate & eventDeleguate, const DeusConnectionEventsType type)
	{
		// Should find or create entry
		DeusEventDeleguateVector& eventlistenersVector = m_eventListeners[type];

		// check duoble registration
		for (auto it = eventlistenersVector.begin(); it != eventlistenersVector.end(); ++it)
		{
			if (eventDeleguate == (*it)) // already registered !
				return false;
		}

		eventlistenersVector.push_back(eventDeleguate);
		return false;
	}

	bool DeusConnection::RemoveListener(const DeusEventDeleguate & eventDeleguate, const DeusConnectionEventsType type)
	{
		// try to find our deleguate list for the given type
		DeusConnectionEventDeleguateMap::iterator matchingType = m_eventListeners.find(type);
		
		if (matchingType != m_eventListeners.end())
		{
			// Get the vector for the event type found
			DeusEventDeleguateVector& listeners = matchingType->second;
			for (auto it = listeners.begin(); it != listeners.end(); ++it)
			{
				if (eventDeleguate == (*it)) // found the listener to del
				{
					listeners.erase(it);
					return true;
				}
			}
		}

		return false;
	}


	bool DeusConnection::TriggerEvent(const DeusCore::PacketSPtr& p_packet, const DeusConnectionEventsType type)
	{
		bool processed = false;

		// try to find our deleguate list for the given type
		DeusConnectionEventDeleguateMap::iterator matchingType = m_eventListeners.find(type);
	
		if (matchingType != m_eventListeners.end())
		{
			// Get the vector for the event type found
			DeusEventDeleguateVector& listeners = matchingType->second;

			// loop and all deleguate and call thems
			for (auto it = listeners.begin(); it != listeners.end(); ++it)
			{
				DeusEventDeleguate deleguate = (*it);
				deleguate(m_id, p_packet); // call the deleguate
				processed = true;
			}
		}

		return processed;
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

