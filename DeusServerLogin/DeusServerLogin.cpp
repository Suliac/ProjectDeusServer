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

	DeusNetwork::Buffer512 buf, recvbuf;

	DeusNetwork::MessageText packet;
	packet.SetMessage("coucou");
	packet.Serialize(buf);

	std::cout << "Serialized size : " << buf.index << std::endl;
	int sentsize = 0;
	try
	{
		sentsize = client.TCPSend(buf);
	}
	catch (const DeusNetwork::DeusSocketException& e)
	{
		std::cout << e.GetErrorMessage() << std::endl;
		return 1;
	}

	std::cout << "bytes sent : " << sentsize << std::endl;

	client.SocketShutdown(); // stop connection (write way)
	int recvsize = 0;
	DeusNetwork::MessageText packetReceived;

	do {
		recvsize = client.TCPRecv(recvbuf);
		if (recvsize > 0)
		{
			std::cout << "Bytes received: " << recvsize << std::endl;
			packetReceived.Deserialize(recvbuf);
			std::cout << "Deserialized size : " << recvbuf.index << std::endl;
			std::cout << "Message : " << packetReceived.GetTextMessage() << std::endl;
		}
		else if (recvsize == 0)
			std::cout << "Connection closed" << std::endl;

	} while (recvsize > 0);

	client.SocketClose();

	return 0;
}

