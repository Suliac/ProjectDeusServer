#pragma once
#include "NetworkServer.h"

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
		virtual void OnUpdate() override;
		virtual void OnStart() override;
		virtual void OnEnd() override;
		virtual void OnInterpretPacket(int senderId, DeusCore::PacketSPtr p_packet) override;
	
		/////////////////////////// 
		//          GAMES        // 
		///////////////////////////
	private:
		unsigned int m_gameId;
	};
	using GameNetworkServerUPtr = std::unique_ptr<GameNetworkServer>;
	using GameNetworkServerSPtr = std::shared_ptr<GameNetworkServer>;
}

