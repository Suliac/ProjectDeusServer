#pragma once
#include "TimeLineComponent.h"
namespace DeusServer
{
	class HealthTimeLineComponent : public TimeLineComponent<int>
	{
	public:
		HealthTimeLineComponent();
		~HealthTimeLineComponent();


	protected:
		virtual void OnStart() override;
		virtual std::shared_ptr<int> Interpolate(const int & beforeValue, uint64_t beforeValueTimestamp, const int & afterValue, uint64_t afterValueTimestamp, uint64_t currentMs) const override;
		virtual std::shared_ptr<int> Extrapolate(const int & beforeValue, uint64_t beforeValueTimestamp, uint64_t currentMs) const override;
	};
}

