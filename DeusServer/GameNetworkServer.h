#pragma once
#include "NetworkServer.h"
#include "DeusCore/EventManagerHandler.h"

namespace DeusServer
{
	class GameNetworkServer : public NetworkServer
	{
	public:
		GameNetworkServer(int gameId);
		~GameNetworkServer();

		bool NewPlayer(int clientId, DeusClientSPtr clientConnection);
	protected:
		/////////////////////////// 
		//         SERVER        // 
		///////////////////////////
		virtual void OnStart() override;
		virtual void OnStop() override;
		virtual void OnDisconnectClient(int clientId) override;
		
		/////////////////////////// 
		//          GAMES        // 
		///////////////////////////
		void InterpretPacket(DeusCore::DeusEventSPtr p_packet);
		void LeaveGame(unsigned int clientId);
		void TryDeleteGame();
	private:
		unsigned int m_gameId;
		unsigned int m_stopped;
	};
	using GameNetworkServerUPtr = std::unique_ptr<GameNetworkServer>;
	using GameNetworkServerSPtr = std::shared_ptr<GameNetworkServer>;
}

