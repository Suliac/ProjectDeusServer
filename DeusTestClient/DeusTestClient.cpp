// DeusTestClient.cpp : définit le point d'entrée pour l'application console.
//

#include "DeusCore/Socket.h"
#include "DeusCore/Buffer.h"
#include "DeusCore/TcpSocket.h"
#include "DeusCore/TcpListener.h"
#include "DeusCore/DeusSocketException.h"

#include "DeusCore/Packet.h"
#include "DeusCore/PacketTest.h"

#include <iostream>
#include <string>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

int main()
{
	DeusNetwork::TcpSocket client;
	try
	{
		client.TCPConnect("127.0.0.1", "27015", true); // server infos
	}
	catch (const DeusNetwork::DeusSocketException& e)
	{
		std::cout << e.GetErrorMessage() << std::endl;
		return 1;
	}

	DeusNetwork::PacketTest packet;
	packet.SetMessage("coucou");

	size_t sentsize = 0;
	try
	{
		// Crado pour test!
		int sendSomething = 0;
		do
		{
			if (client.TCPSend(packet, sentsize))
				sendSomething++;
		} while (sendSomething < 1);
	}
	catch (const DeusNetwork::DeusSocketException& e)
	{
		std::cout << e.GetErrorMessage() << std::endl;
		return 1;
	}

	client.SocketShutdown(); // stop connection (soft way)
	size_t recvsize = 0;

	//std::unique_ptr<DeusNetwork::Packet> p_packetReceived = nullptr;
	//do {
	//	// Crado pour test!
	//	bool receivedSomething = false;
	//	do
	//	{
	//		receivedSomething = client.TCPRecv(p_packetReceived, recvsize);
	//	} while (!receivedSomething);

	//	if (recvsize > 0)
	//		std::cout << "Message : " << ((DeusNetwork::PacketTest*)p_packetReceived.get())->GetTextMessage() << std::endl;
	//	else if (recvsize == 0)
	//		std::cout << "Connection closed" << std::endl;

	//} while (recvsize > 0);

	client.SocketClose();
	return 0;
}



