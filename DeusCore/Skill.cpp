#include "Skill.h"
#include "ResourcesHandler.h"

namespace DeusCore
{
	Skill::Skill()
	{
	}

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
		m_totalTimeMs = castTime; // /!\ cast time is in seconds

		for (const auto& p_effect : m_effects)
			m_totalTimeMs += p_effect->GetDuration();// /!\ duration is in seconds

		m_totalTimeMs *= 1000; // convert in ms
	}
	
	Skill::~Skill()
	{
	}

	void Skill::Deserialize(Buffer512& buffer)
	{
		DeserializeData(buffer, m_id);

		LoadSkill(m_id);
	}

	void Skill::Serialize(Buffer512 & buffer) const
	{
		SerializeData(buffer, m_id);
	}

	uint16_t Skill::EstimateAnswerCurrentSerializedSize() const
	{
		return sizeof(m_id);
	}

	void Skill::LoadSkill(Id skillId)
	{
		SkillPtr p_currentSkill = ResourcesHandler::Instance()->GetSkill(m_id);
		if (p_currentSkill)
		{
			m_name = p_currentSkill->GetName();
			m_isCircle = p_currentSkill->IsCircle();
			m_castTime = p_currentSkill->GetCastTime();
			m_maxScope = p_currentSkill->GetMaxScope();
			m_radius = p_currentSkill->GetRadius();
			m_level = p_currentSkill->GetLevel();
			m_manaCost = p_currentSkill->GetManaCost();
			m_effects = p_currentSkill->GetEffects();
		}
	}

	//-----------------------------------------------------------------------
	SkillEffect::SkillEffect(float effectDuration, float damages)
	{
		m_effectDuration = effectDuration;
		m_damages = damages;
	}

	SkillEffect::~SkillEffect()
	{
	}

	//-----------------------------------------------------------------------
	SkillInfos::SkillInfos()
	{
		m_launchTimeMs = 0;
		m_launchPosition = DeusCore::DeusVector2::Zero();
		//m_skillState = ESkillState::NotLaunched;
	}

	SkillInfos::SkillInfos(const Skill& model, uint32_t launchTime, DeusVector2 position)
		: Skill(model)
	{
		m_launchTimeMs = launchTime;
		m_launchPosition = position;
		//m_skillState = ESkillState::NotLaunched;
	}

	void SkillInfos::Deserialize(Buffer512& buffer)
	{
		Skill::Deserialize(buffer);

		DeserializeData(buffer, m_launchTimeMs);
		DeserializeData<ISerializable>(buffer, m_launchPosition);
	}

	void SkillInfos::Serialize(Buffer512 & buffer) const
	{
		Skill::Serialize(buffer);

		SerializeData(buffer, m_launchTimeMs);
		SerializeData<ISerializable>(buffer, m_launchPosition);
	}

	uint16_t SkillInfos::EstimateAnswerCurrentSerializedSize() const
	{
		return Skill::EstimateAnswerCurrentSerializedSize() + m_launchPosition.EstimateAnswerCurrentSerializedSize() + sizeof(m_launchTimeMs);
	}
}