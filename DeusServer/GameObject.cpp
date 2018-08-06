#include "GameObject.h"
#include "DeusCore/DeusException.h"

namespace DeusServer
{
	GameObject::GameObject(std::vector<GameObjectComponent>& components)
	{
		for (const auto& component : components) {
			if (m_components.find(component.GetId()) != m_components.end())
				throw DeusCore::DeusException("try to insert two components with same id");

			m_components.insert(std::make_pair(component.GetId(), std::make_shared<GameObjectComponent>(component)));
		}
	}
	
	GameObject::~GameObject()
	{
	}

	std::shared_ptr<GameObjectComponent> GameObject::GetComponent(uint32_t identifier)
	{
		GameObjectComponents::iterator componentIt = m_components.find(identifier);
		if (componentIt != m_components.end())
			return componentIt->second;

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
