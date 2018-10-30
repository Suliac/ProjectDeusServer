#pragma once
#include <memory>
#include <vector>
#include <string>

#include "DeusVector2.h"
namespace DeusCore
{
	using Id = uint32_t;

	class SkillEffect
	{
	public:
		SkillEffect(float effectDuration, float damages);
		~SkillEffect();

		float GetDuration() const { return m_effectDuration; }
		float GetDamage() const { return m_damages; }

	private:
		float m_effectDuration; // if 0 => instant
		float m_damages;

		// TODO : Add link for buff/debuff myabe a list of alterations ?
	};

	using SkillEffectPtr = std::shared_ptr<SkillEffect>;

	class Skill : public ISerializable
	{
	public:
		Skill();
		Skill(Id id, std::string name, bool isCircle, float castTime, uint16_t maxScope, uint16_t radius, uint16_t level, uint16_t manaCost, std::vector<SkillEffectPtr>& effects);
		~Skill();

		Id GetId() const { return m_id; }
		std::string GetName() const { return m_name; }
		bool IsCircle() const { return m_isCircle; }
		float GetCastTime() const { return m_castTime; }
		uint16_t GetMaxScope() const { return m_maxScope; }
		uint16_t GetLevel() const { return m_level; }
		uint16_t GetManaCost() const { return m_manaCost; }
		uint16_t GetRadius() const { return m_radius; }
		const std::vector<SkillEffectPtr> GetEffects() const { return m_effects; }

		uint32_t GetTotalTime() const { return m_totalTimeMs; } // calculated on load

		std::string ToString() const { return "ID : " + std::to_string(m_id) + " | Name : " + m_name + " | Shape : " + (m_isCircle ? "Circle" : "Not Circle") + " | Cast time : " + std::to_string(m_castTime) + " | Max scope : " + std::to_string(m_maxScope) + " | Radius : " + std::to_string(m_radius) + " | Level : " + std::to_string(m_level) + " | Mana cost : " + std::to_string(m_manaCost); }
	
		virtual void Deserialize(Buffer512 & buffer) override;
		virtual void Serialize(Buffer512 & buffer) const override;
		virtual uint16_t EstimateAnswerCurrentSerializedSize() const override;
	
	protected:
		void LoadSkill(Id skillId);

		Id m_id;
		std::string m_name;
		bool m_isCircle;

		std::vector<SkillEffectPtr> m_effects;
		float m_castTime;
		uint16_t m_maxScope;
		uint16_t m_radius;
		uint16_t m_level;
		uint16_t m_manaCost;
		uint16_t m_totalTimeMs;

		// Class : warrior/mage ...
	};

	using SkillPtr = std::shared_ptr<Skill>;

	enum ESkillState {
		NotLaunched = 0,
		Casting = 1,
		Launched = 2,
		Finished = 3
	};

	class SkillInfos : public Skill
	{
	public:

		SkillInfos();
		SkillInfos(const Skill& model, uint32_t launchTime, DeusVector2 position);

		uint32_t GetLaunchTime() const { return m_launchTimeMs; }
		//ESkillState GetState() const { return m_skillState; }
		DeusVector2 GetLaunchPosition() const { return m_launchPosition; }

		uint32_t SetLaunchTime(uint32_t value) { m_launchTimeMs = value; }
		//ESkillState SetState(ESkillState value) { m_skillState = value; }
		DeusVector2 SetLaunchPosition(const DeusVector2& value) { m_launchPosition = value; }

		virtual void Deserialize(Buffer512 & buffer) override;
		virtual void Serialize(Buffer512 & buffer) const override;
		virtual uint16_t EstimateAnswerCurrentSerializedSize() const override;

	private:
		uint32_t m_launchTimeMs;
		//ESkillState m_skillState;
		DeusVector2 m_launchPosition;

	};
}
