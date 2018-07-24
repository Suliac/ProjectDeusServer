// DeusTestClient.cpp : définit le point d'entrée pour l'application console.
//

#include "DeusCore/Socket.h"
#include "DeusCore/Buffer.h"
#include "DeusCore/TcpSocket.h"
#include "DeusCore/TcpListener.h"
#include "DeusCore/DeusSocketException.h"
#include "DeusCore/Logger.h"

#include "DeusCore/Packets.h"
#include "DeusCore/Packet.h"
#include "DeusCore/PacketTest.h"
#include "DeusCore/PacketCreateGameAnswer.h"
#include "DeusCore/PacketCreateGameRequest.h"
#include "DeusCore/PacketGetGamesAnswer.h"
#include "DeusCore/PacketGetGamesRequest.h"
#include "DeusCore/PacketJoinGameAnswer.h"
#include "DeusCore/PacketJoinGameRequest.h"

#include <iostream>
#include <string>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

int main()
{
	DeusCore::TcpSocket client;
	try
	{
		client.TCPConnect("127.0.0.1", "27015", true); // server infos
	}
	catch (const DeusCore::DeusSocketException& e)
	{
		DeusCore::Logger::Instance()->Log("Client", e.GetErrorMessage());
		return 1;
	}

	while (true)
	{
		std::unique_ptr<DeusCore::Packet> packet = nullptr;
		std::string message = "";

		std::cout << "choix :" << std::endl;
		std::getline(std::cin, message);
		if (message == "stop")
			break;
		if (message == "0")
		{
			std::cout << "Message :" << std::endl;
			packet = std::unique_ptr<DeusCore::PacketTest>(new DeusCore::PacketTest());
			((DeusCore::PacketTest*)packet.get())->SetMessage(message);
		}
		if (message == "1")
		{
			packet = std::unique_ptr<DeusCore::PacketGetGamesRequest>(new DeusCore::PacketGetGamesRequest());
		}
		else if (message == "2")
		{
			packet = std::unique_ptr<DeusCore::PacketCreateGameRequest>(new DeusCore::PacketCreateGameRequest());
		}
		else if (message == "3")
		{
			std::cout << "Id room :" << std::endl;
			std::getline(std::cin, message);
			packet = std::unique_ptr<DeusCore::PacketJoinGameRequest>(new DeusCore::PacketJoinGameRequest());
			((DeusCore::PacketJoinGameRequest*)packet.get())->SetGameId(atoi(message.c_str()));
		}
		else if (message == "4")
		{
			packet = std::unique_ptr<DeusCore::PacketLeaveGameRequest>(new DeusCore::PacketLeaveGameRequest());
		}

		//packet.SetMessage(message);

		int sentsize, recvsize = 0;
		try
		{
			// Crado pour test!
			int sendSomething = 0;
			do
			{
				if (client.TCPSend(*packet, sentsize))
					sendSomething++;

			} while (sendSomething < 1);
		}
		catch (const DeusCore::DeusSocketException& e)
		{
			DeusCore::Logger::Instance()->Log("Client", e.GetErrorMessage());
			return 1;
		}

		std::unique_ptr<DeusCore::Packet> p_packetReceived = nullptr;

		// Crado pour test!
		bool receivedSomething = false;

		int count = 0;
		do
		{
			count++;
			receivedSomething = client.TCPRecv(p_packetReceived, recvsize);
		} while (!receivedSomething && count < 10000);

		if (count < 10000) // Sale, pour débug
		{
			if (recvsize > 0)
			{
				std::cout << "Received: " << recvsize << " bytes.";
				std::cout << "Packet Type: " << std::to_string(p_packetReceived->GetID());
				switch (p_packetReceived->GetID())
				{
				case DeusCore::Packet::EMessageType::CreateGameAnswer:
					std::cout << "Creation of game: " << (((DeusCore::PacketCreateGameAnswer*)p_packetReceived.get())->GetSuccess() ? "Success" : "Failed") << std::endl;
					break;

				case DeusCore::Packet::EMessageType::JoinGameAnswer:
					std::cout << "Join game: " << ((DeusCore::PacketJoinGameAnswer*)p_packetReceived.get())->GetGameId() << std::endl;
					break;

				case DeusCore::Packet::EMessageType::GetGameAnswer:
					std::cout << "Number of games: " << ((DeusCore::PacketGetGamesAnswer*)p_packetReceived.get())->GetGames().size() << std::endl;

					break;

				default:
					break;
				}
			}

			else if (recvsize == 0)
				std::cout << "Connection closed" << std::endl;
		}

	}

	client.SocketShutdown(); // stop connection (soft way)
	client.SocketClose();
	return 0;
}



