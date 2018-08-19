#include "GameObjectFactory.h"
#include "HealthTimeLineComponent.h"
#include "PositionTimeLineComponent.h"

namespace DeusServer
{
	std::unique_ptr<GameObject> GameObjectFactory::Create(GameObject::EObjectType objectType)
	{
		std::vector<std::shared_ptr<GameObjectComponent>> components;
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

	void GameObjectFactory::GetPlayerComponents(std::vector<std::shared_ptr<GameObjectComponent>>& components)
	{
		components.push_back(std::shared_ptr<HealthTimeLineComponent>(new HealthTimeLineComponent()));
		components.push_back(std::shared_ptr<PositionTimeLineComponent>(new PositionTimeLineComponent()));

	}


}
