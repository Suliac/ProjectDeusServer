#pragma once
#include "DeusCore/TcpListener.h"
#include "DeusCore/EventManagerHandler.h"

#include "NetworkServer.h"
#include "GameHandler.h"

#include <string>
#include <thread>

namespace DeusServer
{
	using DeusGames = std::map<int, std::unique_ptr<GameHandler>>;
	class WorldNetworkServer : public NetworkServer
	{
	public:
		WorldNetworkServer();
		~WorldNetworkServer();

		void SetConnectInfo(std::string ipAddr, std::string port)
		{
			m_ipAddr = ipAddr;
			m_port = port;
		}
	private:

		/////////////////////////// 
		//         SERVER        // 
		/////////////////////////// 

		virtual void OnStart() override;
		virtual void OnStop() override;
		virtual void OnDisconnectClient(int clientId) override;
		//virtual void OnUpdate() override;
		//virtual void OnEnd() override;

		// Threaded function to accept new connection
		void AcceptConnection();
			
		// Threaded function to accept users input
		void HandleInput();

		/////////////////////////// 
		//       DELEGUATES      // 
		///////////////////////////
		
		void InitDeleguate();
		void DeleteDeleguate();
		
		/////////////////////////// 
		//          GAMES        // 
		///////////////////////////

		void InterpretPacket(DeusCore::DeusEventSPtr p_packet);
		void GetGames(int clientId);
		void CreateGame(int clientId);
		void JoinGame(int clientId, int gameId);
		void DeleteGame(int gameId);

		/**************** Attributes ********************/

		/////////////////////////// 
		//         SERVER        // 
		/////////////////////////// 

		// Ip adress of the server
		std::string m_ipAddr;

		// Port of the server
		std::string m_port;

		// Socket that listen TCP connection
		DeusCore::TcpListener m_listener;

		size_t m_nextClientId;

		std::thread m_inputHandlerThread;
		std::thread m_acceptConnectionsThread;
		
		bool m_stopped = false;

		/////////////////////////// 
		//          GAMES        // 
		///////////////////////////
		unsigned int m_nextGameId;
		DeusGames m_games;
		std::mutex m_gamesLocker;
		std::mutex m_connectionsLocker;


		/************************************************/
	};
}

