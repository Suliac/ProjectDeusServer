#include "SkillTimeLineComponent.h"

namespace DeusServer
{
	SkillTimeLineComponent::SkillTimeLineComponent()
		: TimeLineComponent(EComponentType::SkillComponent)
	{		
	}


	SkillTimeLineComponent::~SkillTimeLineComponent()
	{
	}

	std::shared_ptr<DeusCore::SkillInfos> SkillTimeLineComponent::Interpolate(const DeusCore::SkillInfos & beforeValue, uint32_t beforeValueTimestamp, const DeusCore::SkillInfos & afterValue, uint32_t afterValueTimestamp, uint32_t currentMs) const
	{
		return nullptr;
	}

	std::shared_ptr<DeusCore::SkillInfos> SkillTimeLineComponent::Extrapolate(const DeusCore::SkillInfos & beforeValue, uint32_t beforeValueTimestamp, uint32_t currentMs) const
	{
		return nullptr;
	}
}
