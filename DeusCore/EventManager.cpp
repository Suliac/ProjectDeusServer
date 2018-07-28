#include "EventManager.h"
#include "Logger.h"

#include <Windows.h>

namespace DeusCore
{
	EventManager::EventManager()
	{
				
	}

	//---------------------------------------------------------------------------------
	EventManager::EventManager(std::string name)
		: m_name(name)
	{
		m_activeQueue = 0;
		m_runThread = std::thread([this] { Run(); });
		Logger::Instance()->Log(m_name, "Started");
	}

	//---------------------------------------------------------------------------------
	EventManager::~EventManager()
	{
		Stop();
	}

	//---------------------------------------------------------------------------------
	bool EventManager::AddListener(const DeusEventDeleguate & eventDeleguate, const Packet::EMessageType & type)
	{
		// Should find or create entry
		DeusEventDeleguateList& eventlistenersVector = m_eventListeners[type];

		// check doble registration
		for (auto it = eventlistenersVector.begin(); it != eventlistenersVector.end(); ++it)
		{
			if (eventDeleguate == (*it)) // already registered !
				return false;
		}

		eventlistenersVector.push_back(eventDeleguate);
		return true;
	}

	//---------------------------------------------------------------------------------
	bool EventManager::RemoveListener(const DeusEventDeleguate & eventDeleguate, const Packet::EMessageType & type)
	{
		// try to find our deleguate list for the given type
		DeusEventDeleguateMap::iterator matchingType = m_eventListeners.find(type);

		if (matchingType != m_eventListeners.end())
		{
			// Get the vector for the event type found
			DeusEventDeleguateList& listeners = matchingType->second;
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

	//---------------------------------------------------------------------------------
	bool EventManager::TriggerEvent(const DeusEventSPtr& deusEvent)
	{
		assert(deusEvent->second);

		bool processed = false;

		// try to find our deleguate list for the given type
		DeusEventDeleguateMap::iterator matchingList = m_eventListeners.find(deusEvent->second->GetType());

		if (matchingList != m_eventListeners.end())
		{
			// Get the vector for the event type found
			DeusEventDeleguateList listeners = matchingList->second;

			// loop and all deleguate and call thems
			for (auto it = listeners.begin(); it != listeners.end(); ++it)
			{
				DeusEventDeleguate deleguate = (*it);
				deleguate(deusEvent); // call the deleguate
				processed = true;
			}
		}

		return processed;
	}

	//---------------------------------------------------------------------------------
	bool EventManager::QueueEvent(const DeusEventSPtr & deusEvent)
	{
		assert(m_activeQueue >= 0 && m_activeQueue < EVENTMANAGER_NUM_QUEUE);
		assert(deusEvent->second);

		DeusEventDeleguateMap::iterator matchingList = m_eventListeners.find(deusEvent->second->GetType());
		if (matchingList != m_eventListeners.end())
		{
			m_queueLock.lock(); // <---------- Lock

			m_queues[m_activeQueue].push_back(deusEvent);
			m_thereIsPacket = true;

			m_queueLock.unlock(); // <---------- Unlock
			m_waitForPacket.notify_all(); // Notify
			return true;
		}
		else
			return false;
	}

	//---------------------------------------------------------------------------------
	bool EventManager::AbortEvent(const Packet::EMessageType & type, bool allOfType)
	{
		assert(m_activeQueue >= 0 && m_activeQueue < EVENTMANAGER_NUM_QUEUE);
		bool success = false;

		// try to find our deleguate list for the given type
		DeusEventDeleguateMap::iterator matchingType = m_eventListeners.find(type);

		if (matchingType != m_eventListeners.end())
		{
			// Get the vector for the event type found
			m_queueLock.lock(); // <---------- Lock
			DeusEventQueue& eventQueue = m_queues[m_activeQueue];

			auto it = eventQueue.begin();
			while (it != eventQueue.end())
			{
				// removing an item from queue will invalidate the iterator
				// so we create a tmp value and make our interator point on
				// the next item
				auto thisIt = it;
				it++;

				if ((*thisIt)->second->GetType() == type)
				{
					eventQueue.erase(thisIt);
					success = true;

					if (!allOfType)
						return success;
				}
			}

			m_thereIsPacket = m_queues[m_activeQueue].size() > 0;

			m_queueLock.unlock(); // <---------- Unlock
			m_waitForPacket.notify_all(); // Notify
		}

		return success;
	}

	//---------------------------------------------------------------------------------
	void EventManager::Stop()
	{
		if (!m_stopped)
		{
			// Dont forget to set m_wantToStop BEFORE the notify
			// otherwise the thread will not awake if m_thereIsPacket is false
			m_wantToStop = true;
			m_waitForPacket.notify_all();

			if (m_runThread.joinable())
				m_runThread.join();

			m_stopped = true;
		}
		Logger::Instance()->Log(m_name, "Sopped");

	}

	//---------------------------------------------------------------------------------
	void EventManager::Run()
	{
		while (!m_wantToStop)
		{			
			// Concurrent add occurs only in the active queue
			std::unique_lock<std::mutex> locker(m_queueLock);
			m_waitForPacket.wait(locker,
				[this] { return m_thereIsPacket || m_wantToStop; }); // <---------- Wait for packet in the active queue or for stopping the eventmanager

			if (m_wantToStop)
			{
				locker.unlock();
				break;
			}

			unsigned long currentMs = GetTickCount();
			unsigned long maxMs = currentMs + MAX_TIME_PROCESS_MS;

			// swap active queue and then clear it
			int queueToProcess = m_activeQueue;
			m_activeQueue = (m_activeQueue + 1) % EVENTMANAGER_NUM_QUEUE;
			m_queues[m_activeQueue].clear();

			//Logger::Instance()->Log(m_name, "Unlock1");
			locker.unlock(); // <---------- Unlock

			// We don't need to lock here because the processed queue
			// is only used here : the extern packet are comming into the active queue
			// We now process all the 
			while (!m_queues[queueToProcess].empty())
			{
				// pop event
				DeusEventSPtr p_event = m_queues[queueToProcess].front();
				m_queues[queueToProcess].pop_front();
								
				//Logger::Instance()->Log(m_name, "Packet of type :" + std::to_string(p_event->second->GetType())+" | ID sender : "+ std::to_string(p_event->first));
				// We trigger the event
				TriggerEvent(p_event);
				//Logger::Instance()->Log(m_name, "Just triggered type :" + std::to_string(p_event->second->GetType()) + " | ID sender : " + std::to_string(p_event->first));

				currentMs = GetTickCount();
				if (currentMs > MAX_TIME_PROCESS_MS) // we ran out of time
					break;
			}

			// if we couldn't process all event, push the remaining ones
			// to the new active queue.
			if (!m_queues[queueToProcess].empty())
			{
				// Concurrent add occurs only in the active queue
				m_queueLock.lock(); // <---------- Lock
				while (!m_queues[queueToProcess].empty())
				{
					DeusEventSPtr p_event = m_queues[queueToProcess].back();
					m_queues[queueToProcess].pop_back();
					m_queues[m_activeQueue].push_front(p_event);
				}
				m_thereIsPacket = true;
				m_queueLock.unlock(); // <---------- Unlock
			}
			else {
				m_queueLock.lock(); // <---------- Lock
				m_thereIsPacket = false;

				m_queueLock.unlock(); // <---------- Unlock
			}
		}

		Logger::Instance()->Log(m_name, "End run EventManager");
		m_stopped = true;
	}
}
