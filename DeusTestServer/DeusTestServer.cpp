// DeusTestServer.cpp : définit le point d'entrée pour l'application console.
//

#include "DeusCore/Logger.h"
#include "DeusCore/UdpSocket.h"
#include "DeusCore/Packet.h"

int main()
{
	//std::string addr = "127.0.0.1", port = "27015";

	////Init listener
	//std::cout << "Init and launch listener" << std::endl;
	//DeusCore::TcpListener listener;
	//try {
	//	listener.Init(addr, port, true);
	//	listener.Start();
	//}
	//catch (const DeusCore::DeusSocketException& e)
	//{
	//	std::cout << e.GetErrorMessage() << std::endl;
	//	return 1;
	//}

	//// Accept 1 connection
	//DeusCore::TcpSocket client;
	//try {
	//	do {
	//	} while (!listener.Accept(client));
	//}
	//catch (const DeusCore::DeusSocketException& e)
	//{
	//	throw e;
	//}

	//std::cout << "Connection accepted !" << std::endl;

	//// Read message
	//size_t sendedLength = 0, recvlength = 0;
	//DeusCore::PacketUPtr p_packetReceived = nullptr;
	//do {

	//	try
	//	{
	//		// Crado pour test!
	//		bool receivedSomething = false;
	//		do
	//		{
	//			receivedSomething = client.TCPRecv(p_packetReceived, recvlength);
	//		} while (!receivedSomething);
	//	}
	//	catch (const DeusCore::DeusSocketException& e)
	//	{
	//		std::cout << e.GetErrorMessage() << std::endl;
	//		return 1;
	//	}

	//	if (recvlength > 0)
	//	{
	//		std::cout << "Message : " << ((DeusCore::PacketTest*)p_packetReceived.get())->GetTextMessage() << std::endl;

	//		// Echo the buffer back to the sender// Crado pour test!
	//		bool sendSomething = false;
	//		do
	//		{
	//			sendSomething = client.TCPSend(*((DeusCore::PacketTest*)p_packetReceived.get()), sendedLength);
	//		} while (!sendSomething);
	//	}
	//	else if (recvlength == 0) // no data anymore
	//	{
	//		std::cout << "Connection closing...\n" << std::endl;
	//	}
	//	else {
	//		std::cout << "recv failed: " << WSAGetLastError() << std::endl;
	//		client.SocketClose();
	//		return 1;
	//	}

	//} while (recvlength > 0);

	//client.SocketShutdown();
	//client.SocketClose();
	DeusCore::UdpSocket socket;
	socket.Init("127.0.0.1", "27015", false, false);

	int sentLength = 0, recvlength = 0;
	bool wantToStop = false;
	char datas[512];

	DeusCore::Logger::Instance()->Log("Test UDP Serv", "Server Started");

	do
	{
		if (socket.RecvDatas(datas, 512, recvlength))
		{
			DeusCore::Logger::Instance()->Log("Test UDP Serv", "Datas recv");
			if (recvlength == 0)
			{
				wantToStop = true;
				break;
			}
			else if (recvlength > 0)
			{
				DeusCore::Logger::Instance()->Log("Test UDP Serv", std::to_string(recvlength) + " bytes recv");
				// echo back
				socket.SendDatas(datas, recvlength, sentLength);
				DeusCore::Logger::Instance()->Log("Test UDP Serv", std::to_string(recvlength) + " bytes sent");
			}
		}
	} while (!wantToStop);

	socket.SocketClose();
	return 0;
}