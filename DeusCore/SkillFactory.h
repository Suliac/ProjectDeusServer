#pragma once
#include "Skill.h"
#include "tinyxml2.h"
namespace DeusCore
{
	class SkillFactory
	{
	public:
		SkillFactory();
		~SkillFactory();

		bool LoadSkills(const std::string path, std::vector<SkillPtr>& skills) const;

	private:
		SkillPtr CreateSkillFromXML(tinyxml2::XMLElement* p_element) const;
		SkillEffectPtr CreateSkillEffectFromXml(tinyxml2::XMLElement* p_effect) const;
	};
}

