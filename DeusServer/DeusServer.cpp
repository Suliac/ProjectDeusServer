// DeusServer.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"
#include "Socket.h"
#include "TcpSocket.h"
#include "TcpListener.h"

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

int main(int argc, char **argv)
{
	if (argc != 3) {
		printf("usage: %s [ip_adress_server] [port_server] \n", argv[0]);
		return 1;
	}

	// Init listener
	printf("Init and launch listener\n");
	DeusNetwork::TcpListener listener;
	listener.Init(argv[1], argv[2]);
	listener.Start();
	
	// Accept 1 connection
	SOCKET connectedSocket = listener.Accept();
	if (connectedSocket == INVALID_SOCKET)
	{
		printf("Error while retrieving the connection with client");
		return 1;
	}

	printf("Connection accepted !\n");

	DeusNetwork::TcpSocket client;
	client.TCPConnect(connectedSocket);

	// Read message
	char recvbuf[512];
	int recvbuflen = 512;
	int receivedLength, sendedLength;
	
	do {
		receivedLength = client.TCPRecv(recvbuf, recvbuflen);
		if (receivedLength > 0)
		{
			printf("Bytes received: %d\n", receivedLength);

			// Echo the buffer back to the sender
			sendedLength = client.TCPSend(recvbuf, receivedLength);
			if (sendedLength == SOCKET_ERROR)
			{
				client.SocketClose();
				return 1;
			}
			printf("Bytes sent: %d\n", sendedLength);
		}
		else if (receivedLength == 0) // plus de données
		{
			printf("Connection closing...\n");
		}
		else {
			printf("recv failed: %d\n", WSAGetLastError());
			client.SocketClose();
			return 1;
		}

	} while (receivedLength > 0);

	client.SocketShutdown();
	client.SocketClose();

    return 0;
}

