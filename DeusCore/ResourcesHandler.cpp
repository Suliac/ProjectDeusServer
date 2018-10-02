#include "ResourcesHandler.h"
#include "Logger.h"
namespace DeusCore
{
	ResourcesHandler* ResourcesHandler::p_instance = nullptr;
	std::mutex ResourcesHandler::m_instanceResourceLock;

	/////////////////////////
	//     SINGLETON	   //
	/////////////////////////
	ResourcesHandler* ResourcesHandler::Instance()
	{
		if (p_instance == nullptr) {
			m_instanceResourceLock.lock();
			if (p_instance == nullptr) {
				p_instance = new ResourcesHandler;
			}
			m_instanceResourceLock.unlock();
		}

		return p_instance;
	}

	void ResourcesHandler::Free() {
		if (p_instance != nullptr) {
			m_instanceResourceLock.lock();
			if (p_instance != nullptr) {
				delete p_instance;
			}
			m_instanceResourceLock.unlock();
		}

		m_instanceResourceLock.try_lock();
		m_instanceResourceLock.unlock();
	}

	ResourcesHandler::ResourcesHandler()
	{
		SkillFactory factory;
		Logger::Instance()->Log("RES HANDLER", "Start Loading Skills");
		if (!factory.LoadSkills("../DeusServer/Resources/skills.xml", m_skills))
			throw std::exception("Cannot load skills");

		Logger::Instance()->Log("RES HANDLER", std::to_string(m_skills.size()) + " skills loaded");
	}

	ResourcesHandler::~ResourcesHandler()
	{
	}

}