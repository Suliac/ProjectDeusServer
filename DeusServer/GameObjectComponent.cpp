#include "GameObjectComponent.h"


namespace DeusServer
{
	Id GameObjectComponent::NextId = 1;

	GameObjectComponent::GameObjectComponent(EComponentType type)
	{

		m_uniqueIdentifier = NextId;
		NextId++;
		m_type = type;
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
