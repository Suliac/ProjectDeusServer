#include "HealthTimeLineComponent.h"

namespace DeusServer
{
	HealthTimeLineComponent::HealthTimeLineComponent()
	{
	}


	HealthTimeLineComponent::~HealthTimeLineComponent()
	{
	}
	
	int HealthTimeLineComponent::GetValue()
	{		
		// Interpolate the health of an object is very simple : 
		// just take the last amount we got for any datas saved before the timestamp
		long ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		return GetValueAtTime(ms);
	}

	void HealthTimeLineComponent::OnStart()
	{
		// init with default value
		InsertData(0, 0);
	}
}
