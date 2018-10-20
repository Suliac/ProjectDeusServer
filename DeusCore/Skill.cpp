#include "Skill.h"


namespace DeusCore
{
	Skill::Skill(Id id, std::string name, bool isCircle, float castTime, uint16_t maxScope, uint16_t radius, uint16_t level, uint16_t manaCost, std::vector<SkillEffectPtr>& effects)
		: m_id(id),
		m_name(name),
		m_isCircle(isCircle),
		m_castTime(castTime),
		m_maxScope(maxScope),
		m_radius(radius),
		m_level(level),
		m_manaCost(manaCost),
		m_effects(std::move(effects))
	{
	}


	Skill::~Skill()
	{
	}

	SkillEffect::SkillEffect(float effectDuration, float damages)
	{
		m_effectDuration = effectDuration;
		m_damages = damages;
	}

	SkillEffect::~SkillEffect()
	{
	}

}