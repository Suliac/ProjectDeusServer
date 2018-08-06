#pragma once
#include "IExecutable.h"
#include "IIdentifiable.h"
#include <memory>
namespace DeusServer
{
	class GameObjectComponent : public IExecutable, public IIdentifiable
	{
	public:
		GameObjectComponent();
		~GameObjectComponent();

	protected:
		virtual void OnUpdate(double deltatimeMs) override;
		virtual void OnStart() override;
		virtual void OnStop() override;

	};
}

