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
		DeusCore::Logger::Instance()->Log(m_name, "New game. ID : " + std::to_string(gameId));

		// Init Game Network Server
		mp_gameNetServer = new GameNetworkServer(gameId);
	}

	GameHandler::~GameHandler()
	{
		delete mp_gameNetServer;
		DeusCore::Logger::Instance()->Log(m_name, "Delete " + m_name);
	}

	bool GameHandler::NewPlayer(int clientId, DeusClientSPtr clientConnection)
	{
		if (!mp_gameNetServer)
			return false;

		return mp_gameNetServer->NewPlayer(clientId, clientConnection);
		return false;
	}


}
