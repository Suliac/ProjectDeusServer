#pragma once
#include "TimeLineComponent.h"
namespace DeusServer
{
	class HealthTimeLineComponent : public TimeLineComponent<int>
	{
	public:
		HealthTimeLineComponent(Id parentObjectId);
		~HealthTimeLineComponent();


	protected:
		//virtual void OnStart() override;
		virtual std::shared_ptr<int> Interpolate(const int & beforeValue, uint32_t beforeValueTimestamp, const int & afterValue, uint32_t afterValueTimestamp, uint32_t currentMs) const override;
		virtual std::shared_ptr<int> Extrapolate(const int & beforeValue, uint32_t beforeValueTimestamp, uint32_t currentMs) const override;
	};
}

