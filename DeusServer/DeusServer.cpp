// DeusServer.cpp : définit le point d'entrée pour l'application console.
//

// #include "stdafx.h"
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
	size_t sendedLength, recvlength;
	//DeusNetwork::Packet * p_packetReceived = nullptr;
	std::unique_ptr<DeusNetwork::Packet> p_packetReceived = nullptr;
	do {

		try
		{
			p_packetReceived = client.TCPRecv(recvlength);
		}
		catch (const DeusNetwork::DeusSocketException& e)
		{
			std::cout << e.GetErrorMessage() << std::endl;
			return 1;
		}

		if (recvlength > 0)
		{
			std::cout << "Message : " << ((DeusNetwork::PacketTest*)p_packetReceived.get())->GetTextMessage() << std::endl;

			// Echo the buffer back to the sender
			client.TCPSend(*((DeusNetwork::PacketTest*)p_packetReceived.get()), sendedLength);
		}
		else if (recvlength == 0) // no data anymore
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

