#pragma once
#include "TimeLineComponent.h"
namespace DeusServer
{
	class HealthTimeLineComponent : public TimeLineComponent<int>
	{
	public:
		HealthTimeLineComponent();
		~HealthTimeLineComponent();

		virtual void OnStart() override;

	protected:
		virtual std::shared_ptr<int> Interpolate(const int & beforeValue, long beforeValueTimestamp, const int & afterValue, long afterValueTimestamp, long currentMs) const override;
		virtual std::shared_ptr<int> Extrapolate(const int & beforeValue, long beforeValueTimestamp, long currentMs) const override;
	};
}

