#pragma once
#include "IExecutable.h"
#include "IIdentifiable.h"

#include "DeusCore/ISerializable.h"

#include <memory>
#include <mutex>
namespace DeusServer
{
	const uint32_t DELAY_MS = 200;
	class GameObjectComponent : public IExecutable, public IIdentifiable
	{
	public:
		enum EComponentType : uint8_t
		{
			Error				= 0,
			HealthComponent		= 1,
			PositionComponent	= 2,
			SkillComponent		= 3,
		};

		GameObjectComponent(EComponentType type);
		~GameObjectComponent();

		EComponentType GetType() const { return m_type; }
	protected:
		virtual void OnUpdate(double deltatimeMs) override;
		virtual void OnStart() override;
		virtual void OnStop() override;
		
	private:
		static Id NextId;
		EComponentType m_type;
	};
}

