#pragma once
#include "IExecutable.h"
#include "IIdentifiable.h"
#include <memory>
#include <mutex>
namespace DeusServer
{
	const uint32_t DELAY_MS = 200;
	class GameObjectComponent : public IExecutable, public IIdentifiable
	{
	public:
		
		GameObjectComponent();
		~GameObjectComponent();

	protected:
		virtual void OnUpdate(double deltatimeMs) override;
		virtual void OnStart() override;
		virtual void OnStop() override;

	private:
		static Id NextId;

	};
}

