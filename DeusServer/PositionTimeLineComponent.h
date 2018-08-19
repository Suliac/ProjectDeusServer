#pragma once
#include "TimeLineComponent.h"
#include "DeusCore/DeusVector2.h"
namespace DeusServer
{
	struct DeusObjectMovement {
	public:
		DeusCore::DeusVector2 Dir;
		DeusCore::DeusVector2 Position;

		DeusObjectMovement()
		{
			Dir = DeusCore::DeusVector2::Zero();
			Position = DeusCore::DeusVector2::Zero();
		}

		DeusObjectMovement(DeusCore::DeusVector2 dir, DeusCore::DeusVector2 originPosition)
		{
			Dir = dir;
			Position = originPosition;
		}

	};

	class PositionTimeLineComponent : public TimeLineComponent<DeusCore::DeusVector2>
	{
	public:
		PositionTimeLineComponent();
		~PositionTimeLineComponent();
		
	protected:
		virtual std::shared_ptr<DeusCore::DeusVector2> Interpolate(const DeusCore::DeusVector2& beforeValue, long beforeValueTimestamp, const DeusCore::DeusVector2& afterValue, long afterValueTimestamp, long currentMs) const override;
		virtual std::shared_ptr<DeusCore::DeusVector2> Extrapolate(const DeusCore::DeusVector2& beforeValue, long beforeValueTimestamp, long currentMs) const override;

	};
}

