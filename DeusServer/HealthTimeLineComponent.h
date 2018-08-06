#pragma once
#include "TimeLineComponent.h"
namespace DeusServer
{
	class HealthTimeLineComponent : public TimeLineComponent<int>
	{
	public:
		HealthTimeLineComponent();
		~HealthTimeLineComponent();

		virtual int GetValue() override;
		virtual void OnStart() override;
	};
}

