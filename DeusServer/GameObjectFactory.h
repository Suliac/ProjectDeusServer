#pragma once
#include "GameObject.h"
namespace DeusServer
{
	class GameObjectFactory
	{
	public:
		static std::unique_ptr<GameObject> Create(GameObject::EObjectType objectType);

	private:
		static void GetPlayerComponents(std::vector<GameObjectComponent>& components);
	};
}

