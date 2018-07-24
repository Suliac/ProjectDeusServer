#include "GameHandler.h"
#include "GameNetworkServer.h"

#include "DeusCore/Packets.h"
#include "DeusCore/Logger.h"

namespace DeusServer
{
	GameHandler::GameHandler(int gameId)
		: m_gameId(gameId)
	{
		// Debug
		m_name = m_name + std::to_string(gameId);
		DeusCore::Logger::Instance()->Log(m_name, "New " + m_name);

		// Init Game Network Server
		mp_gameNetServer = new GameNetworkServer(gameId);
		mp_gameNetServer->Start(gameId);
	}

	//---------------------------------------------------------------------------------
	GameHandler::~GameHandler()
	{
		if (mp_gameNetServer != nullptr)
			delete mp_gameNetServer;

		DeusCore::Logger::Instance()->Log(m_name, "Delete " + m_name);
	}

	//---------------------------------------------------------------------------------
	void GameHandler::Stop()
	{
		mp_gameNetServer->Stop();
	}



}
