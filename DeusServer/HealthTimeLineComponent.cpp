#include "HealthTimeLineComponent.h"

namespace DeusServer
{
	HealthTimeLineComponent::HealthTimeLineComponent()
	{
	}


	HealthTimeLineComponent::~HealthTimeLineComponent()
	{
	}
	
	void HealthTimeLineComponent::OnStart()
	{
		// init with default value
		InsertData(std::make_shared<int>(0), 0);
	}
	std::shared_ptr<int> HealthTimeLineComponent::Interpolate(const int & beforeValue, uint32_t beforeValueTimestamp, const int & afterValue, uint32_t afterValueTimestamp, uint32_t currentMs) const
	{
		// Interpolate the health of an object is very simple : 
		// just take the last amount we got for any datas saved before the timestamp
		int value = beforeValue;
		return std::make_shared<int>(value);
	}
	std::shared_ptr<int> HealthTimeLineComponent::Extrapolate(const int & beforeValue, uint32_t beforeValueTimestamp, uint32_t currentMs) const
	{
		int value = beforeValue;
		return std::make_shared<int>(value);
	}
}
