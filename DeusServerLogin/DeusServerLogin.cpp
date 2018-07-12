// DeusServerLogin.cpp : définit le point d'entrée pour l'application console.
//

#include "DeusCore/Socket.h"
#include "DeusCore/Buffer.h"
#include "DeusCore/TcpSocket.h"
#include "DeusCore/TcpListener.h"
#include "DeusCore/DeusSocketException.h"
#include "DeusCore/Message.h"
#include "DeusCore/MessageText.h"

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
		client.TCPConnect("127.0.0.1", "27015"); // server infos
	}
	catch (const DeusNetwork::DeusSocketException& e)
	{
		std::cout << e.GetErrorMessage() << std::endl;
		return 1;
	}
	
	DeusNetwork::MessageText packet;
	packet.SetMessage("coucou");

	size_t sentsize = 0;
	try
	{
		client.TCPSend(packet, sentsize);
	}
	catch (const DeusNetwork::DeusSocketException& e)
	{
		std::cout << e.GetErrorMessage() << std::endl;
		return 1;
	}
	
	client.SocketShutdown(); // stop connection (soft way)
	size_t recvsize = 0;
	DeusNetwork::MessageText packetReceived;

	do {
		client.TCPRecv(packetReceived, recvsize);
		if (recvsize > 0)
		{
			std::cout << "Message : " << packetReceived.GetTextMessage() << std::endl;
		}
		else if (recvsize == 0)
			std::cout << "Connection closed" << std::endl;

	} while (recvsize > 0);

	client.SocketClose();

	return 0;
}

