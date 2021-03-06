#pragma once
#include "GameObject.h"
namespace DeusServer
{
	class GameObjectFactory
	{
	public:
		static std::unique_ptr<GameObject> Create(GameObject::EObjectType objectType, Id gameId);

	private:
		static void GetPlayerComponents(std::vector<std::shared_ptr<GameObjectComponent>>& components, Id parentObjectId, Id gameId);
	};
}

