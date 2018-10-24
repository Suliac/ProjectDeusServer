#include "GameObjectComponent.h"


namespace DeusServer
{
	Id GameObjectComponent::NextId = 1;

	GameObjectComponent::GameObjectComponent(EComponentType type, Id parentObjectId)
	{
		m_uniqueIdentifier = NextId;
		NextId++;

		m_type = type;
		m_parentObjectId = parentObjectId;
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
