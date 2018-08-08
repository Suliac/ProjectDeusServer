#pragma once
#include "DeusClient.h"
#include "GameLogicServer.h"

namespace DeusServer
{
	class GameNetworkServer;

	using DeusClientConnections = std::map<Id, std::shared_ptr<DeusServer::DeusClient>>;
	
	class GameHandler
	{
	public:
		GameHandler() {};
		GameHandler(int gameId);
		~GameHandler();
		
		void Stop();

	private:
		// Unique pointer on the Game Network Server
		GameNetworkServer* mp_gameNetServer = nullptr;

		std::unique_ptr<GameLogicServer> mp_gameLogicServer = nullptr;

		// Id of the game
		int m_gameId = -1;
		
		// Debug name
		std::string m_name = "Game Manager";
	};
}

