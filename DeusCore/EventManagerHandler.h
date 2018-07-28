#pragma once
#include <mutex>
#include <map>

#include "EventManager.h"
namespace DeusCore
{
	using GamesEventManagers = std::map<unsigned int, std::unique_ptr<EventManager>>;
	
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

		bool StartGameChannel	(unsigned int gameId);
		bool StopGameChannel	(unsigned int gameId);
		
		// Event manager methods

		bool AddListener	(unsigned int gameId, const DeusEventDeleguate& eventDeleguate, const Packet::EMessageType& type);
		bool RemoveListener	(unsigned int gameId, const DeusEventDeleguate& eventDeleguate, const Packet::EMessageType& type);
		bool TriggerEvent	(unsigned int gameId, const DeusEventSPtr& deusEvent);
		bool AbortEvent		(unsigned int gameId, const Packet::EMessageType& type, bool allOfType = false);
		bool QueueEvent		(unsigned int gameId, const DeusEventSPtr& deusEvent);
		bool QueueEvent		(unsigned int gameId, int senderId, PacketSPtr p_packet);


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

