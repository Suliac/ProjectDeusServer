#pragma once

#include "TimeLineComponent.h"

#include "DeusCore/Skill.h"
namespace DeusServer
{
	class SkillTimeLineComponent : public TimeLineComponent<DeusCore::SkillInfos>
	{
	public:
		SkillTimeLineComponent();
		~SkillTimeLineComponent();

	protected:
		virtual std::shared_ptr<DeusCore::SkillInfos> Interpolate(const DeusCore::SkillInfos & beforeValue, uint32_t beforeValueTimestamp, const DeusCore::SkillInfos & afterValue, uint32_t afterValueTimestamp, uint32_t currentMs) const override;
		virtual std::shared_ptr<DeusCore::SkillInfos> Extrapolate(const DeusCore::SkillInfos & beforeValue, uint32_t beforeValueTimestamp, uint32_t currentMs) const override;
	};
}
