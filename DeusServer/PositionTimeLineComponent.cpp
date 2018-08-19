#include "PositionTimeLineComponent.h"


namespace DeusServer
{
	PositionTimeLineComponent::PositionTimeLineComponent()
	{
	}


	PositionTimeLineComponent::~PositionTimeLineComponent()
	{
	}

	void PositionTimeLineComponent::OnStart()
	{
		InsertData(std::make_shared<const DeusCore::DeusVector2>(DeusCore::DeusVector2::Zero()), 0);
	}

	std::shared_ptr<DeusCore::DeusVector2> PositionTimeLineComponent::Interpolate(const DeusCore::DeusVector2& beforeValue, uint64_t beforeValueTimestamp, const DeusCore::DeusVector2& afterValue, uint64_t afterValueTimestamp, uint64_t currentMs) const
	{
		DeusCore::DeusVector2 value = DeusCore::DeusVector2::Zero();

		// pos = start + (end-start) x (time_elsapsed_since_start / duration_bewteen_start_and_end)
		value = beforeValue + (afterValue - beforeValue) * (float)((float)(currentMs - beforeValueTimestamp) / (float)(afterValueTimestamp - beforeValueTimestamp));

		return std::make_shared<DeusCore::DeusVector2>(value);
	}

	std::shared_ptr<DeusCore::DeusVector2> PositionTimeLineComponent::Extrapolate(const DeusCore::DeusVector2 & beforeValue, uint64_t beforeValueTimestamp, uint64_t currentMs) const
	{
		return std::make_shared<DeusCore::DeusVector2>(beforeValue);
	}
}
