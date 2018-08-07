#pragma once
#include "FastDelegate.h"
#include "Packet.h"

#include <list>
#include <map>
#include <thread>
#include <mutex>

namespace DeusCore
{

	// A deus event is the id of the sender with the packet he sends
	using DeusEvent = std::pair<Id, DeusCore::PacketSPtr>;
	using DeusEventSPtr = std::shared_ptr<DeusEvent>;

	using DeusEventDeleguate = fastdelegate::FastDelegate1<DeusEventSPtr>;
	using DeusEventDeleguateList = std::list<DeusEventDeleguate>;
	using DeusEventDeleguateMap = std::map<Packet::EMessageType, DeusEventDeleguateList>;

	using DeusEventQueue = std::list<DeusEventSPtr>;

	const unsigned int EVENTMANAGER_NUM_QUEUE = 2;
	const unsigned int MAX_TIME_PROCESS_MS = 2000; // 2 sec

	class EventManager // Inspired by EventManager of Game Coding Complete (MacShaffry & Graham)
	{
	public:
		EventManager();
		EventManager(std::string name);
		~EventManager();

		bool AddListener(const DeusEventDeleguate& eventDeleguate, const Packet::EMessageType& type);
		bool RemoveListener(const DeusEventDeleguate& eventDeleguate, const Packet::EMessageType& type);
		bool TriggerEvent(const DeusEventSPtr& deusEvent);
		bool QueueEvent(const DeusEventSPtr& deusEvent);
		bool AbortEvent(const Packet::EMessageType& type, bool allOfType = false);

		// Stop our EventManager
		void Stop();
	private:
		// Entry point of our thread, get events and trigger deleguate as fast as possible
		void Run();

		// We want our eventmanager to run on its own thread
		std::thread m_runThread;

		// Do we want to stop the execution ?
		bool m_wantToStop = false;
		bool m_stopped = false;

		// Al deleaguete listener mapped with their trigger event type
		DeusEventDeleguateMap m_eventListeners;

		// Array of queue, we use 2 queue to have a doble buffer
		DeusEventQueue m_queues[EVENTMANAGER_NUM_QUEUE];

		std::mutex m_queueLock;
		std::condition_variable m_waitForPacket;
		bool m_thereIsPacket = false;

		// index of actively processing queue
		// We enqueue to the opposite queue
		int m_activeQueue;

		std::string m_name;
	};
}

