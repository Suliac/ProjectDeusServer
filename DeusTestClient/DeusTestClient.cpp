// DeusTestClient.cpp : définit le point d'entrée pour l'application console.
//

#include "DeusCore/Socket.h"
#include "DeusCore/TcpSocket.h"
#include "DeusCore/UdpSocket.h"
#include "DeusCore/TcpListener.h"
#include "DeusCore/DeusSocketException.h"
#include "DeusCore/Logger.h"

#include "DeusCore/Packets.h"


int main()
{

	// Initialize WSA
	WSADATA m_wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &m_wsaData);
	if (iResult != 0) {
		return 1;
	}

	DeusCore::Logger::Instance()->Log("Client", "launch client");
	// -------------------------------------------
	DeusCore::UdpSocket udpClient;
	DeusCore::TcpSocket tcpClient;
	try
	{
		tcpClient.TCPConnect("127.0.0.1", "27015", true); // server infos

		DeusCore::Logger::Instance()->Log("Client", "Client connected in TCP");
		bool tcpReceivedSomething = false;
		int recvTcpSize = 0;

		std::unique_ptr<DeusCore::Packet> p_tcpPacket = nullptr;
		do
		{
			tcpReceivedSomething = tcpClient.Recv(p_tcpPacket, recvTcpSize);
		} while (recvTcpSize <= 0);

		DeusCore::Logger::Instance()->Log("Client", "Get TCP respoonse");
		std::string portUdp;
		std::string addrUdp;
		if (p_tcpPacket->GetType() == DeusCore::Packet::EMessageType::Connected)
		{
			addrUdp = ((DeusCore::PacketClientConnected*)p_tcpPacket.get())->GetAddrUdp();
			portUdp = std::to_string(((DeusCore::PacketClientConnected*)p_tcpPacket.get())->GetPortUdp());
			std::cout << "addr :" << ((DeusCore::PacketClientConnected*)p_tcpPacket.get())->GetAddrUdp() << "port :" << std::to_string(((DeusCore::PacketClientConnected*)p_tcpPacket.get())->GetPortUdp()) << std::endl;
		
		}
		else {
			throw DeusCore::DeusException("Impossible de récupérer les données UDP");
		}

		udpClient.Init(addrUdp, portUdp, true, true);
	}
	catch (const DeusCore::DeusSocketException& e)
	{
		DeusCore::Logger::Instance()->Log("Client", e.GetErrorMessage());
		return 1;
	}

	/*try
	{*/

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

		DeusCore::Logger::Instance()->Log("Client", "Try send the message with id : " + std::to_string(packet->GetId()));

		int sentsize, recvsize = 0;
		try
		{
			// Crado pour test!
			int sendSomething = 0;
			do
			{
				if (udpClient.Send(*packet, sentsize))
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

		try {

			DeusCore::Logger::Instance()->Log("Client", "Try to read");
			long start = GetTickCount();
			long elapsed = start;
			while (elapsed - start < (long)5000)
			{
				do
				{
					receivedSomething = udpClient.Recv(p_packetReceived, recvsize);
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
					elapsed = GetTickCount();
				} while (!receivedSomething && elapsed - start < (long)5000);

				if (receivedSomething)
				{
					if (recvsize > 0)
					{
						//DeusCore::Logger::Instance()->Log("Client", "Packet Type: " + std::to_string(p_packetReceived->GetType())+ " | size: " + std::to_string(recvsize));
						switch (p_packetReceived->GetType())
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

						case DeusCore::Packet::EMessageType::Ack:
							std::cout << "RECV ACK for : " << ((DeusCore::PacketAck*)p_packetReceived.get())->GetPacketToAck() << std::endl;
							break;

						default:
							break;
						}

						if (p_packetReceived->GetType() != DeusCore::Packet::EMessageType::Ack)
						{
							packet = std::unique_ptr<DeusCore::PacketAck>(new DeusCore::PacketAck());
							((DeusCore::PacketAck*)packet.get())->SetPacketToAck(p_packetReceived->GetId());
							udpClient.Send(*packet, sentsize);
							std::cout << "SENT ACK for : " << p_packetReceived->GetId() << std::endl;
						}
					}
					else if (recvsize == 0)
						std::cout << "Connection closed" << std::endl;
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(1));
				elapsed = GetTickCount();
			}


		}
		catch (const DeusCore::DeusSerializationException& e)
		{
			DeusCore::Logger::Instance()->Log("Client ", "Not blocking error : " + e.GetErrorMessage());
		}
	}

	/*}
	catch (std::exception& ex)
	{
		DeusCore::Logger::Instance()->Log("Client ", "Blocking error : ");

	}*/

	udpClient.SocketShutdown(); // stop connection (soft way)
	udpClient.SocketClose();

	//----------------------------------------------------------------------
	/*DeusCore::UdpSocket socket;
	socket.Init("127.0.0.1", "27015", true);

	int sentLength = 0, recvlength = 0;
	char datas[512];

	while (true)
	{
		std::string message = "";

		std::cout << "choix :" << std::endl;
		std::getline(std::cin, message);
		if (message == "stop")
			break;

		if (socket.Send(message.c_str(), message.size(), sentLength))
		{
			DeusCore::Logger::Instance()->Log("Test UDP Cli", std::to_string(sentLength) + " bytes sent");
		}

		if (socket.Recv(datas, 512, recvlength))
		{
			DeusCore::Logger::Instance()->Log("Test UDP Cli", std::to_string(recvlength) + " bytes recv");

		}
	}*/

	WSACleanup();
	return 0;
}



