#pragma once
#include "GameObjectComponent.h"
#include "ISerializableComponent.h"
#include <memory>
#include <unordered_map>
namespace DeusServer
{
	using GameObjectComponents = std::unordered_map<uint32_t, std::shared_ptr<GameObjectComponent>>;
	class GameObject : public IExecutable, public IIdentifiable
	{
	public:
		enum EObjectType : uint8_t {
			Anything = 0,
			Player = 1,
		};

		GameObject();
		GameObject(std::vector<std::shared_ptr<GameObjectComponent>>& components, EObjectType typeObject);
		~GameObject();

		void GetSerializableComponents(std::vector<std::shared_ptr<ISerializableComponent>>& components) const;
		std::shared_ptr<GameObjectComponent> GetComponent(uint32_t identifier) const;
		std::shared_ptr<GameObjectComponent> GetFirstComponent(GameObjectComponent::EComponentType componentType) const;
		EObjectType GetType() const { return m_type; }

		static Id GetNextId() { return NextId; }
	protected:
		virtual void OnUpdate(double deltatimeMs) override;
		virtual void OnStart() override;
		virtual void OnStop() override;

		GameObjectComponents m_components;
		EObjectType m_type;

		static Id NextId;
	};
}

