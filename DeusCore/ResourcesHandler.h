#pragma once
#include <mutex>
#include "SkillFactory.h"
namespace DeusCore
{
	class ResourcesHandler
	{
	public:
		/////////////////////////
		//     SINGLETON	   //
		/////////////////////////
		static ResourcesHandler* Instance();
		static void Free();

		SkillPtr GetSkill(Id skill);
	private:
		ResourcesHandler();
		~ResourcesHandler();

		static ResourcesHandler* p_instance;
		static std::mutex m_instanceResourceLock;

		std::vector<SkillPtr> m_skills;
	};
}

