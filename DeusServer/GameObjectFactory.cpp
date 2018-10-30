#include "GameObjectFactory.h"
#include "HealthTimeLineComponent.h"
#include "PositionTimeLineComponent.h"
#include "SkillTimeLineComponent.h"

namespace DeusServer
{
	std::unique_ptr<GameObject> GameObjectFactory::Create(GameObject::EObjectType objectType, Id gameId)
	{
		std::vector<std::shared_ptr<GameObjectComponent>> components;
		switch (objectType)
		{
		case DeusServer::GameObject::Player:
			GetPlayerComponents(components, GameObject::GetNextId(), gameId);
			break;
		default:
			break;
		}

		std::unique_ptr<GameObject> gameObj = std::make_unique<GameObject>(GameObject(components, objectType));
		return gameObj;
	}

	void GameObjectFactory::GetPlayerComponents(std::vector<std::shared_ptr<GameObjectComponent>>& components, Id parentObjectId, Id gameId)
	{
		components.push_back(std::make_shared<HealthTimeLineComponent>(parentObjectId));
		components.push_back(std::make_shared<PositionTimeLineComponent>(parentObjectId));
		components.push_back(std::make_shared<SkillTimeLineComponent>(gameId, parentObjectId));
	}


}
