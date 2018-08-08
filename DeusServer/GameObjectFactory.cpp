#include "GameObjectFactory.h"
#include "HealthTimeLineComponent.h"

namespace DeusServer
{
	std::unique_ptr<GameObject> GameObjectFactory::Create(GameObject::EObjectType objectType)
	{
		std::vector<GameObjectComponent> components;
		switch (objectType)
		{
		case DeusServer::GameObject::Player:
			GetPlayerComponents(components);
			break;
		default:
			break;
		}

		std::unique_ptr<GameObject> gameObj = std::make_unique<GameObject>(GameObject(components, objectType));
		return gameObj;
	}

	void GameObjectFactory::GetPlayerComponents(std::vector<GameObjectComponent>& components)
	{
		components.push_back(HealthTimeLineComponent());
	}


}
