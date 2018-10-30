#include "GameObject.h"
#include "HealthTimeLineComponent.h"
#include "PositionTimeLineComponent.h"
#include "SkillTimeLineComponent.h"
#include "SerializablePositionComponent.h"
#include "SerializableHealthComponent.h"
#include "SerializableSkillComponent.h"

#include "DeusCore/DeusException.h"
#include "DeusCore/DeusVector2.h"
#include "DeusCore/Skill.h"
namespace DeusServer
{
	Id GameObject::NextId = 1;

	GameObject::GameObject()
	{
		m_uniqueIdentifier = NextId;
		NextId++;
	}

	GameObject::GameObject(std::vector<std::shared_ptr<GameObjectComponent>>& components, EObjectType typeObject)
	{
		for (const auto& component : components) {
			if (m_components.find(component->GetId()) != m_components.end())
				throw DeusCore::DeusException("try to insert two components with same id");

			m_components.insert(std::make_pair(component->GetId(), component));
		}

		m_type = typeObject;

		m_uniqueIdentifier = NextId;
		NextId++;
	}

	GameObject::~GameObject()
	{
	}



	void GameObject::GetSerializableComponents(std::vector<std::shared_ptr<ISerializableComponent>>& components) const
	{
		for (const auto& component : m_components)
		{
			uint32_t originMs = 0;
			uint32_t destMs = 0;

			std::shared_ptr<ISerializableComponent> serializableCompo = nullptr;
			uint32_t currentMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

			switch (component.second->GetType())
			{
			case GameObjectComponent::EComponentType::HealthComponent:
			{
				const std::shared_ptr<const int>originValue = std::dynamic_pointer_cast<HealthTimeLineComponent>(component.second)->GetValueAtTime(currentMs, originMs, true);
				const std::shared_ptr<const int>destValue = std::dynamic_pointer_cast<HealthTimeLineComponent>(component.second)->GetValueAtTime(currentMs, destMs, false);

				serializableCompo = std::make_shared<SerializableHealthComponent>(component.second->GetId(),
					component.second->GetType(),
					originValue, originMs,
					destValue, destMs);

				break;
			}
			case GameObjectComponent::EComponentType::PositionComponent:
			{
				const std::shared_ptr<const DeusCore::DeusVector2> originValue = std::dynamic_pointer_cast<PositionTimeLineComponent>(component.second)->GetValueAtTime(currentMs, originMs, true);
				const std::shared_ptr<const DeusCore::DeusVector2> destValue = std::dynamic_pointer_cast<PositionTimeLineComponent>(component.second)->GetValueAtTime(currentMs, destMs, false);

				serializableCompo = std::make_shared<SerializablePositionComponent>(component.second->GetId(),
					component.second->GetType(),
					originValue, originMs,
					destValue, destMs);
				break;
			}
			case GameObjectComponent::EComponentType::SkillComponent:
			{
				const std::shared_ptr<const DeusCore::SkillInfos> originValue = std::dynamic_pointer_cast<SkillTimeLineComponent>(component.second)->GetValueAtTime(currentMs, originMs, true);
				const std::shared_ptr<const DeusCore::SkillInfos> destValue = std::dynamic_pointer_cast<SkillTimeLineComponent>(component.second)->GetValueAtTime(currentMs, destMs, false);

				serializableCompo = std::make_shared<SerializableSkillComponent>(component.second->GetId(),
					component.second->GetType(),
					originValue, originMs,
					destValue, destMs);

				break;
			}
			default:
				break;
			}
			components.push_back(serializableCompo);
		}
	}

	std::shared_ptr<GameObjectComponent> GameObject::GetComponent(uint32_t identifier) const
	{
		for (const auto& component : m_components)
		{
			if (component.second->GetId() == identifier)
				return component.second;
		}

		return nullptr;
	}

	std::shared_ptr<GameObjectComponent> GameObject::GetFirstComponent(GameObjectComponent::EComponentType componentType) const
	{
		for (const auto& component : m_components)
		{
			if (component.second->GetType() == componentType)
				return component.second;
		}

		return nullptr;
	}

	void GameObject::OnUpdate(double deltatimeMs)
	{
		for (const auto& component : m_components) {
			component.second->Update(deltatimeMs);
		}
	}

	void GameObject::OnStart()
	{
		for (const auto& component : m_components) {
			component.second->Start();
		}
	}

	void GameObject::OnStop()
	{
		for (const auto& component : m_components) {
			component.second->Stop();
		}
	}
}
