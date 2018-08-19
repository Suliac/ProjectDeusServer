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
		virtual std::shared_ptr<int> Interpolate(const int & beforeValue, unsigned long beforeValueTimestamp, const int & afterValue, unsigned long afterValueTimestamp, unsigned long currentMs) const override;
		virtual std::shared_ptr<int> Extrapolate(const int & beforeValue, unsigned long beforeValueTimestamp, unsigned long currentMs) const override;
	};
}

