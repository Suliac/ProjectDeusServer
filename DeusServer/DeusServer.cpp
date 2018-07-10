// DeusServer.cpp : définit le point d'entrée pour l'application console.
//

// #include "stdafx.h"
#include "../DeusCore/Socket.h"
#include "../DeusCore/TcpSocket.h"
#include "../DeusCore/TcpListener.h"
#include "../DeusCore/DeusSocketException.h"

#include <iostream>
#include <string>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

int main()
{
	std::string addr = "127.0.0.1", port = "27015";

	// Init listener
	std::cout << "Init and launch listener" << std::endl;
	DeusNetwork::TcpListener listener;
	try {
		listener.Init(addr, port);
		listener.Start();
	}
	catch (DeusNetwork::DeusSocketException e)
	{
		std::cout << e.GetErrorMessage() << std::endl;
		getchar();
		return 1;
	}

	// Accept 1 connection
	DeusNetwork::TcpSocket client;
	try {
		listener.Accept(client);
	}
	catch (DeusNetwork::DeusSocketException e)
	{
		throw e;
	}

	std::cout << "Connection accepted !" << std::endl;

	// Read message
	char recvbuf[512];
	int recvbuflen = 512;
	int receivedLength, sendedLength;

	do {
		receivedLength = client.TCPRecv(recvbuf, recvbuflen);
		if (receivedLength > 0)
		{
			std::cout << "Bytes received: " << receivedLength << std::endl;

			// Echo the buffer back to the sender
			sendedLength = client.TCPSend(recvbuf, receivedLength);
			if (sendedLength == SOCKET_ERROR)
			{
				client.SocketClose();
				return 1;
			}
			std::cout << "Bytes sent: " << sendedLength << std::endl;
		}
		else if (receivedLength == 0) // plus de données
		{
			std::cout << "Connection closing...\n" << std::endl;
		}
		else {
			std::cout << "recv failed: " << WSAGetLastError() << std::endl;
			client.SocketClose();
			return 1;
		}

	} while (receivedLength > 0);

	client.SocketShutdown();
	client.SocketClose();

	return 0;
}

