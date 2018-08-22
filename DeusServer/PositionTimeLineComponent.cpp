#include "PositionTimeLineComponent.h"
#include "DeusCore/Logger.h"


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

	std::shared_ptr<DeusCore::DeusVector2> PositionTimeLineComponent::Interpolate(const DeusCore::DeusVector2& beforeValue, uint32_t beforeValueTimestamp, const DeusCore::DeusVector2& afterValue, uint32_t afterValueTimestamp, uint32_t currentMs) const
	{
		DeusCore::DeusVector2 value = DeusCore::DeusVector2::Zero();

		// pos = start + (end-start) x (time_elsapsed_since_start / duration_bewteen_start_and_end)
		DeusCore::DeusVector2 distance = (afterValue - beforeValue);
		float dtTime = (float)((float)(currentMs - beforeValueTimestamp) / (float)(afterValueTimestamp - beforeValueTimestamp));
		DeusCore::DeusVector2 distanceTime = (distance * dtTime);
		value = beforeValue + distanceTime;

		DeusCore::Logger::Instance()->Log("Component", "BEFORE Pos Y: " + std::to_string(beforeValue.Y()));
		DeusCore::Logger::Instance()->Log("Component", "AFTER  Pos Y: " + std::to_string(afterValue.Y()));
		if (value.Y() < 0)
		{
			DeusCore::Logger::Instance()->Log("Component", "************************************");
			DeusCore::Logger::Instance()->Log("Component", "distance Pos Y: " + std::to_string(distance.Y()));
			DeusCore::Logger::Instance()->Log("Component", "dtTime : "+ std::to_string(dtTime));
			DeusCore::Logger::Instance()->Log("Component", "distance * dtTime = " + std::to_string(distanceTime.Y()));
			DeusCore::Logger::Instance()->Log("Component", "beforeValue + distance * dtTime = " + std::to_string((beforeValue + distanceTime).Y()));
			DeusCore::Logger::Instance()->Log("Component", "beforeValue + distance * dtTime = " + std::to_string(value.Y()));
			DeusCore::Logger::Instance()->Log("Component", "************************************");

		}


		DeusCore::Logger::Instance()->Log("Component", "[INTERPOLATION] Pos : " + std::to_string(value.X()) + "," + std::to_string(value.Y()));
		return std::make_shared<DeusCore::DeusVector2>(value);
	}

	std::shared_ptr<DeusCore::DeusVector2> PositionTimeLineComponent::Extrapolate(const DeusCore::DeusVector2 & beforeValue, uint32_t beforeValueTimestamp, uint32_t currentMs) const
	{
		DeusCore::Logger::Instance()->Log("Component", "[EXTRAPOLATION]");
		return std::make_shared<DeusCore::DeusVector2>(beforeValue);
	}
}
