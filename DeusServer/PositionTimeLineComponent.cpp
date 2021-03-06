#include "PositionTimeLineComponent.h"
#include "DeusCore/Logger.h"


namespace DeusServer
{
	PositionTimeLineComponent::PositionTimeLineComponent(Id parentObjectId)
		: TimeLineComponent(EComponentType::PositionComponent, parentObjectId)
	{
		InsertData(std::make_shared<const DeusCore::DeusVector2>(DeusCore::DeusVector2::Zero()));
	}


	PositionTimeLineComponent::~PositionTimeLineComponent()
	{
	}

	/*void PositionTimeLineComponent::OnStart()
	{
		
	}*/

	std::shared_ptr<DeusCore::DeusVector2> PositionTimeLineComponent::Interpolate(const DeusCore::DeusVector2& beforeValue, uint32_t beforeValueTimestamp, const DeusCore::DeusVector2& afterValue, uint32_t afterValueTimestamp, uint32_t currentMs) const
	{
		DeusCore::DeusVector2 value = DeusCore::DeusVector2::Zero();

		// pos = start + (end-start) x (time_elsapsed_since_start / duration_bewteen_start_and_end)
		float dtTime = (float)((float)(currentMs - beforeValueTimestamp) / (float)(afterValueTimestamp - beforeValueTimestamp));
		value = beforeValue + (afterValue - beforeValue) * dtTime;

		return std::make_shared<DeusCore::DeusVector2>(value);
	}

	std::shared_ptr<DeusCore::DeusVector2> PositionTimeLineComponent::Extrapolate(const DeusCore::DeusVector2 & beforeValue, uint32_t beforeValueTimestamp, uint32_t currentMs) const
	{
		return std::make_shared<DeusCore::DeusVector2>(beforeValue);
	}
}
