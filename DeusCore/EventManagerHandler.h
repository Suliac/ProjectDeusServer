#pragma once
#include <mutex>
#include <map>

#include "EventManager.h"
namespace DeusCore
{
	using GamesEventManagers = std::map<Id, std::unique_ptr<EventManager>>;
	
	class EventManagerHandler
	{
	public:
		/////////////////////////
		//     SINGLETON	   //
		/////////////////////////
		static EventManagerHandler* Instance();
		static void Free();
		/////////////////////////
		//   IMPLEMENTATION	   //
		/////////////////////////
		// Channel management

		bool StartGameChannel	(Id gameId);
		bool StopGameChannel	(Id gameId);
		
		// Event manager methods

		bool AddListener	(Id gameId, const DeusEventDeleguate& eventDeleguate, const Packet::EMessageType& type);
		bool RemoveListener	(Id gameId, const DeusEventDeleguate& eventDeleguate, const Packet::EMessageType& type);
		bool TriggerEvent	(Id gameId, const DeusEventSPtr& deusEvent);
		bool AbortEvent		(Id gameId, const Packet::EMessageType& type, bool allOfType = false);
		bool QueueEvent		(Id gameId, const DeusEventSPtr& deusEvent);
		bool QueueEvent		(Id gameId, Id senderId, PacketSPtr p_packet);


	private:
		/////////////////////////
		//     SINGLETON	   //
		/////////////////////////
		EventManagerHandler();
		~EventManagerHandler();

		static EventManagerHandler* p_instance;
		static std::mutex m_instanceLock;

		/////////////////////////
		//   IMPLEMENTATION	   //
		/////////////////////////
		GamesEventManagers m_gamesEventManagers;
		std::mutex m_gamesEventsManagersLock;
	};
}

