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
		virtual void OnStart() override;
		virtual std::shared_ptr<DeusCore::DeusVector2> Interpolate(const DeusCore::DeusVector2& beforeValue, uint64_t beforeValueTimestamp, const DeusCore::DeusVector2& afterValue, uint64_t afterValueTimestamp, uint64_t currentMs) const override;
		virtual std::shared_ptr<DeusCore::DeusVector2> Extrapolate(const DeusCore::DeusVector2& beforeValue, uint64_t beforeValueTimestamp, uint64_t currentMs) const override;

	};
}

