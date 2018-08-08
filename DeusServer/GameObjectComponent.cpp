#include "GameObjectComponent.h"


namespace DeusServer
{
	Id GameObjectComponent::NextId = 1;

	GameObjectComponent::GameObjectComponent()
	{
		m_uniqueIdentifier = NextId;
		NextId++;
	}

	GameObjectComponent::~GameObjectComponent()
	{
	}

	void GameObjectComponent::OnUpdate(double deltatimeMs)
	{
	}

	void GameObjectComponent::OnStart()
	{
	}

	void GameObjectComponent::OnStop()
	{
	}
}
