#pragma once
#include "DeusCore/TcpListener.h"

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

		/*void Run(std::string ipAddr = "127.0.0.1", std::string port = "27015");*/
		void SetConnectInfo(std::string ipAddr, std::string port)
		{
			m_ipAddr = ipAddr;
			m_port = port;
		}
	private:

		/////////////////////////// 
		//         SERVER        // 
		/////////////////////////// 
		virtual void OnUpdate() override;

		virtual void OnStart() override;

		virtual void OnEnd() override;

		virtual void OnInterpretPacket(int senderId, DeusCore::PacketSPtr p_packet) override;

		// Accept new connection
		bool AcceptConnection(const unsigned int id, const unsigned int timeoutSecond = DEFAULT_SOCKETSTATE_TIMEOUT);
			
		// Threaded function to accept users input
		void HandleInput();

		/////////////////////////// 
		//          GAMES        // 
		///////////////////////////
		void GetGames(int clientId);
		void CreateGame(int clientId);
		void JoinGame(int clientId, int gameId);
				

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

		size_t m_nextClientId = 1;

		std::thread m_inputHandlerThread;
		
		/////////////////////////// 
		//          GAMES        // 
		///////////////////////////
		unsigned int m_nextGameId;
		DeusGames m_games;

		/************************************/
	};
}

