#pragma once
#include "GameObjectComponent.h"

#include <memory>
#include <unordered_map>
namespace DeusServer
{
	using GameObjectComponents = std::unordered_map<uint32_t, std::shared_ptr<GameObjectComponent>>;
	class GameObject : public IExecutable, public IIdentifiable
	{
	public:
		enum EObjectType : uint8_t {
			Player = 1,
		};

		GameObject() {};
		GameObject(std::vector<GameObjectComponent>& components);
		~GameObject();

		std::shared_ptr<GameObjectComponent> GetComponent(uint32_t identifier);
	protected:
		virtual void OnUpdate(double deltatimeMs) override;
		virtual void OnStart() override;
		virtual void OnStop() override;

		GameObjectComponents m_components;

	};
}

