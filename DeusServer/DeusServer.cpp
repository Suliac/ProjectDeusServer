// DeusServer.cpp : définit le point d'entrée pour l'application console.
//

// #include "stdafx.h"
#include "../DeusCore/Socket.h"
#include "../DeusCore/Buffer.h"
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

	 //Init listener
	std::cout << "Init and launch listener" << std::endl;
	DeusNetwork::TcpListener listener;
	try {
		listener.Init(addr, port);
		listener.Start();
	}
	catch (const DeusNetwork::DeusSocketException& e)
	{
		std::cout << e.GetErrorMessage() << std::endl;
		return 1;
	}

	// Accept 1 connection
	DeusNetwork::TcpSocket client;
	try {
		listener.Accept(client);
	}
	catch (const DeusNetwork::DeusSocketException& e)
	{
		throw e;
	}

	std::cout << "Connection accepted !" << std::endl;

	// Read message
	DeusNetwork::Buffer512 buf;

	int sendedLength, recvlength;

	do {
		
		try
		{
			recvlength = client.TCPRecv(buf);
		}
		catch (const DeusNetwork::DeusSocketException& e)
		{
			std::cout << e.GetErrorMessage() << std::endl;
			return 1;
		}
		if (recvlength > 0)
		{
			std::cout << "Bytes received: " << recvlength << std::endl;

			buf.index = recvlength;
			// Echo the buffer back to the sender
			sendedLength = client.TCPSend(buf);
			if (sendedLength == SOCKET_ERROR)
			{
				client.SocketClose();
				return 1;
			}
			std::cout << "Bytes sent: " << sendedLength << std::endl;
		}
		else if (recvlength == 0) // plus de données
		{
			std::cout << "Connection closing...\n" << std::endl;
		}
		else {
			std::cout << "recv failed: " << WSAGetLastError() << std::endl;
			client.SocketClose();
			return 1;
		}

	} while (recvlength > 0);

	client.SocketShutdown();
	client.SocketClose();

	return 0;
}

