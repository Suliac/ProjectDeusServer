#include "PositionTimeLineComponent.h"


namespace DeusServer
{
	PositionTimeLineComponent::PositionTimeLineComponent()
	{
	}


	PositionTimeLineComponent::~PositionTimeLineComponent()
	{
	}

	std::shared_ptr<DeusCore::DeusVector2> PositionTimeLineComponent::Interpolate(const DeusCore::DeusVector2& beforeValue, long beforeValueTimestamp, const DeusCore::DeusVector2& afterValue, long afterValueTimestamp, long currentMs) const
	{
		DeusCore::DeusVector2 value = DeusCore::DeusVector2::Zero();

		// pos = start + (end-start) x (time_elsapsed_since_start / duration_bewteen_start_and_end)
		value = beforeValue + (afterValue - beforeValue) * (float)((float)(currentMs - beforeValueTimestamp) / (float)(afterValueTimestamp - beforeValueTimestamp));

		return std::make_shared<DeusCore::DeusVector2>(value);
	}

	std::shared_ptr<DeusCore::DeusVector2> PositionTimeLineComponent::Extrapolate(const DeusCore::DeusVector2 & beforeValue, long beforeValueTimestamp, long currentMs) const
	{
		return std::shared_ptr<DeusCore::DeusVector2>();
	}
}
