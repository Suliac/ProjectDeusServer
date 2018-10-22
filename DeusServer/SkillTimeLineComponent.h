#pragma once

#include "TimeLineComponent.h"

#include "DeusCore/Skill.h"
#include "DeusCore/EventManagerHandler.h"
namespace DeusServer
{
	class SkillTimeLineComponent : public TimeLineComponent<DeusCore::SkillInfos>
	{
	public:
		SkillTimeLineComponent(DeusCore::Id gameId);
		~SkillTimeLineComponent();

	protected:
		virtual void OnStart() override;
		virtual void OnUpdate(double deltatimeMs) override;
		virtual void OnStop() override;

		virtual std::shared_ptr<DeusCore::SkillInfos> Interpolate(const DeusCore::SkillInfos & beforeValue, uint32_t beforeValueTimestamp, const DeusCore::SkillInfos & afterValue, uint32_t afterValueTimestamp, uint32_t currentMs) const override;
		virtual std::shared_ptr<DeusCore::SkillInfos> Extrapolate(const DeusCore::SkillInfos & beforeValue, uint32_t beforeValueTimestamp, uint32_t currentMs) const override;
	
	private:
		void InterpretPacket(DeusCore::DeusEventSPtr p_packet);

		DeusCore::Id m_gameId;

		DeusCore::ESkillState m_lastState;
		size_t m_effectIndex = 0;
		uint32_t m_effectDurationDone = 0;
	};
}
