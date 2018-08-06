#include "IExecutable.h"

void DeusServer::IExecutable::Update(double deltatimeMs)
{
	if (!m_isStopped)
		OnUpdate(deltatimeMs);
}

void DeusServer::IExecutable::Start()
{
	m_isStopped = false;
	OnStart();
}

void DeusServer::IExecutable::Stop()
{
	OnStop();
	m_isStopped = true;
}
