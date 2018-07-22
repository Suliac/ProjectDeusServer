#include "GameNetworkServer.h"

#include "DeusCore/Logger.h"
#include "DeusCore/PacketJoinGameAnswer.h"

namespace DeusServer
{
	GameNetworkServer::GameNetworkServer(int gameId)
	{
		m_gameId = gameId;
		m_name = "Game Server " + std::to_string(gameId);
		DeusCore::Logger::Instance()->Log(m_name, "Started");
	}
	
	GameNetworkServer::~GameNetworkServer()
	{
		DeusCore::Logger::Instance()->Log(m_name, "Deleted");
	}

	/////////////////////////// 
	//         SERVER        // 
	///////////////////////////

	void GameNetworkServer::OnUpdate()
	{
	}
	void GameNetworkServer::OnStart()
	{
	}
	void GameNetworkServer::OnEnd()
	{
	}
	void GameNetworkServer::OnInterpretPacket(int senderId, DeusCore::PacketSPtr p_packet)
	{
	}

	/////////////////////////// 
	//          GAMES        // 
	///////////////////////////

	bool GameNetworkServer::NewPlayer(int clientId, DeusClientSPtr clientConnection)
	{
		DeusClientConnections::iterator findClientId = m_clientsConnections.find(clientId);
		if (findClientId != m_clientsConnections.end())
			return false; // check if client is already here
		
		DeusCore::Logger::Instance()->Log(m_name, "New client : " + std::to_string(clientId));

		// Insert new client into our map
		m_clientsConnections.insert(std::make_pair(clientId, clientConnection));

		// manage deleguates
		SubscribeToBasicEvents(clientId);

		// Send answer
		std::unique_ptr<DeusCore::PacketJoinGameAnswer> p_packet = std::unique_ptr<DeusCore::PacketJoinGameAnswer>(new DeusCore::PacketJoinGameAnswer());
		p_packet->SetGame(m_gameId);
		p_packet->SetSuccess(true);
		SendPacket(std::move(p_packet), clientId, true);
		return true;
	}

}
