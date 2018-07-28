#include <string>

#include "EventManagerHandler.h"
#include "Logger.h"

namespace DeusCore
{
	EventManagerHandler* EventManagerHandler::p_instance = nullptr;
	std::mutex EventManagerHandler::m_instanceLock;

	/////////////////////////
	//     SINGLETON	   //
	/////////////////////////
	//---------------------------------------------------------------------------------
	EventManagerHandler* EventManagerHandler::Instance()
	{
		if (p_instance == nullptr) {
			m_instanceLock.lock();
			if (p_instance == nullptr) {
				EventManagerHandler* temp = new EventManagerHandler;
				p_instance = temp;
			}
			m_instanceLock.unlock();
		}

		return p_instance;
	}

	void EventManagerHandler::Free() {
		if (p_instance != nullptr) {
			m_instanceLock.lock();
			if (p_instance != nullptr) {
				delete p_instance;
			}
			m_instanceLock.unlock();
		}
	}

	//---------------------------------------------------------------------------------
	EventManagerHandler::EventManagerHandler()
	{
	}

	//---------------------------------------------------------------------------------
	EventManagerHandler::~EventManagerHandler()
	{
		m_gamesEventManagers.clear();
	}

	/////////////////////////
	//   IMPLEMENTATION	   //
	/////////////////////////

	//---------------------------------------------------------------------------------
	bool EventManagerHandler::StartGameChannel(unsigned int gameId)
	{
		m_gamesEventsManagersLock.lock(); //<------- Lock
		GamesEventManagers::iterator findIt = m_gamesEventManagers.find(gameId);
		if (findIt == m_gamesEventManagers.end())
		{
			m_gamesEventManagers.insert(std::make_pair(gameId, std::move(std::unique_ptr<EventManager>(new EventManager("Event Mngr " + std::to_string(gameId))))));
			m_gamesEventsManagersLock.unlock(); //<------- Unlock
			return true;
		}
		m_gamesEventsManagersLock.unlock(); //<------- Unlock
		return false;
	}

	//---------------------------------------------------------------------------------
	bool EventManagerHandler::StopGameChannel(unsigned int gameId)
	{
		m_gamesEventsManagersLock.lock(); //<------- Lock

		GamesEventManagers::iterator findIt = m_gamesEventManagers.find(gameId);
		if (findIt != m_gamesEventManagers.end())
		{
			findIt->second->Stop();

			m_gamesEventManagers.erase(findIt);
			m_gamesEventsManagersLock.unlock(); //<------- Unlock
			return true;
		}
		m_gamesEventsManagersLock.unlock(); //<------- Unlock
		return false;
	}

	//---------------------------------------------------------------------------------
	bool EventManagerHandler::AddListener(unsigned int gameId, const DeusEventDeleguate & eventDeleguate, const Packet::EMessageType & type)
	{
		m_gamesEventsManagersLock.lock(); //<------- Lock
		bool success = false;
		GamesEventManagers::iterator findIt = m_gamesEventManagers.find(gameId);
		if (findIt != m_gamesEventManagers.end())
		{
			success = findIt->second->AddListener(eventDeleguate, type);
		}
		m_gamesEventsManagersLock.unlock(); //<------- Unlock
		return success;
	}

	//---------------------------------------------------------------------------------
	bool EventManagerHandler::RemoveListener(unsigned int gameId, const DeusEventDeleguate & eventDeleguate, const Packet::EMessageType & type)
	{
		m_gamesEventsManagersLock.lock(); //<------- Lock
		bool success = false;
		GamesEventManagers::iterator findIt = m_gamesEventManagers.find(gameId);
		if (findIt != m_gamesEventManagers.end())
		{
			success = findIt->second->RemoveListener(eventDeleguate, type);
		}
		m_gamesEventsManagersLock.unlock(); //<------- Unlock
		return success;
	}

	//---------------------------------------------------------------------------------
	bool EventManagerHandler::TriggerEvent(unsigned int gameId, const DeusEventSPtr & deusEvent)
	{
		m_gamesEventsManagersLock.lock(); //<------- Lock
		bool success = false;
		GamesEventManagers::iterator findIt = m_gamesEventManagers.find(gameId);
		if (findIt != m_gamesEventManagers.end())
		{
			success = findIt->second->TriggerEvent(deusEvent);
		}
		m_gamesEventsManagersLock.unlock(); //<------- Unlock
		return success;
	}

	//---------------------------------------------------------------------------------
	bool EventManagerHandler::QueueEvent(unsigned int gameId, const DeusEventSPtr & deusEvent)
	{
		m_gamesEventsManagersLock.lock(); //<------- Lock
		bool success = false;
		GamesEventManagers::iterator findIt = m_gamesEventManagers.find(gameId);
		if (findIt != m_gamesEventManagers.end())
		{
			success = findIt->second->QueueEvent(deusEvent);
		}
		m_gamesEventsManagersLock.unlock(); //<------- Unlock
		return success;
	}

	bool EventManagerHandler::QueueEvent(unsigned int gameId, int senderId, PacketSPtr p_packet)
	{
		return QueueEvent(gameId, std::shared_ptr<DeusEvent>(new std::pair<int, DeusCore::PacketSPtr>(senderId, p_packet)));
	}

	//---------------------------------------------------------------------------------
	bool EventManagerHandler::AbortEvent(unsigned int gameId, const Packet::EMessageType & type, bool allOfType)
	{
		m_gamesEventsManagersLock.lock(); //<------- Lock
		bool success = false;
		GamesEventManagers::iterator findIt = m_gamesEventManagers.find(gameId);
		if (findIt != m_gamesEventManagers.end())
		{
			success = findIt->second->AbortEvent(type, allOfType);
		}
		m_gamesEventsManagersLock.unlock(); //<------- Unlock
		return success;
	}


}
