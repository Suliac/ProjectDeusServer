#include "SkillFactory.h"
#include "Logger.h"

#include <sstream>
#include <assert.h>

namespace DeusCore
{
	SkillFactory::SkillFactory()
	{
	}


	SkillFactory::~SkillFactory()
	{
	}

	bool SkillFactory::LoadSkills(const std::string path, std::vector<SkillPtr>& skills) const
	{
		tinyxml2::XMLDocument doc;
		doc.LoadFile(path.c_str());

		if (doc.ErrorID() == tinyxml2::XMLError::XML_SUCCESS)
		{
			tinyxml2::XMLElement* p_root = doc.FirstChildElement("skill");
			for (tinyxml2::XMLElement* p_element = p_root; p_element != nullptr; p_element = p_element->NextSiblingElement())
			{
				SkillPtr p_newSkill = CreateSkillFromXML(p_element);
				Logger::Instance()->Log("SKILL FACTORY", "SKILL LOADED : " + p_newSkill->ToString());
				skills.push_back(p_newSkill);
			}

			return true;
		}
		else {
			Logger::Instance()->Log("SKILL FACTORY", "Cannot load skills | Error : " + std::to_string(doc.ErrorID()) + " | Path : " + path);
			return false;
		}

	}

	SkillPtr SkillFactory::CreateSkillFromXML(tinyxml2::XMLElement * p_element) const
	{
		assert(p_element != nullptr);
		assert(p_element->FirstChildElement("id") != nullptr);
		assert(p_element->FirstChildElement("name") != nullptr);
		assert(p_element->FirstChildElement("shape") != nullptr);
		assert(p_element->FirstChildElement("casttime") != nullptr);
		assert(p_element->FirstChildElement("scope") != nullptr);
		assert(p_element->FirstChildElement("level") != nullptr);
		assert(p_element->FirstChildElement("manacost") != nullptr);
		assert(p_element->FirstChildElement("effects") != nullptr);


		// Get ID
		Id id = 0;
		std::stringstream sstream;
		sstream << p_element->FirstChildElement("id")->GetText();
		sstream >> id;

		sstream.str("");
		sstream.clear();

		// Get Name
		std::string name = p_element->FirstChildElement("name")->GetText();

		// Get Shape
		bool isCircle = std::string(p_element->FirstChildElement("shape")->GetText()) == std::string("circle");

		// Get Cast time
		float castTime = 0.f;
		sstream << p_element->FirstChildElement("casttime")->GetText();
		sstream >> castTime;

		sstream.str("");
		sstream.clear();

		// Get Scope
		uint16_t scope = 0;
		sstream << p_element->FirstChildElement("scope")->GetText();
		sstream >> scope;

		sstream.str("");
		sstream.clear();

		// Get Level
		uint16_t lvl = 0;
		sstream << p_element->FirstChildElement("level")->GetText();
		sstream >> lvl;

		sstream.str("");
		sstream.clear();

		// Get Mana cost
		uint16_t manaCost = 0;
		sstream << p_element->FirstChildElement("manacost")->GetText();
		sstream >> manaCost;

		sstream.str("");
		sstream.clear();

		// Get Effects
		std::vector<SkillEffectPtr> effects;
		tinyxml2::XMLElement* p_rootEffect = p_element->FirstChildElement("effects");
		for (tinyxml2::XMLElement* p_effect = p_rootEffect->FirstChildElement("effect"); p_effect != nullptr; p_effect = p_effect->NextSiblingElement())
		{
			if (p_effect != nullptr)
				effects.push_back(CreateSkillEffectFromXml(p_effect));
		}

		// Create Skill
		return std::make_shared<Skill>(id, name, isCircle, castTime, scope, lvl, manaCost, effects);
	}

	SkillEffectPtr SkillFactory::CreateSkillEffectFromXml(tinyxml2::XMLElement * p_effect) const
	{
		assert(p_effect->FirstChildElement("duration") != nullptr);
		assert(p_effect->FirstChildElement("damages") != nullptr);

		float duration = 0.f;
		std::stringstream sstream;
		sstream << p_effect->FirstChildElement("duration")->GetText();
		sstream >> duration;

		sstream.str("");
		sstream.clear();

		float damages = 0.f;
		sstream << p_effect->FirstChildElement("damages")->GetText();
		sstream >> duration;

		sstream.str("");
		sstream.clear();

		return std::make_shared<SkillEffect>(duration, damages);
	}
}
