// DeusServer.cpp : définit le point d'entrée pour l'application console.
//

//#include "DeusCore/DeusSocketException.h"

#include "DeusCore/Logger.h"
#include "WorldServer.h"

//#include <iostream>
//#include <string>
//#include <ws2tcpip.h>
//#include <stdlib.h>
//#include <map>
//#include <iomanip>

// Need to link with Ws2_32.lib
//#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

int main()
{
	//bool wantToContinue = true;
	//std::string addr = "127.0.0.1", port = "27015";
	//std::map<int, std::unique_ptr<DeusServer::DeusClient>> clientsConnections;

	//DeusCore::TcpListener listener;

	////Init listener
	//std::cout << "[Server " << std::setw(3) << "" << "] Init and launch listener" << std::endl;
	//try {
	//	listener.Init(addr, port, true);
	//	listener.Start();
	//}
	//catch (const DeusCore::DeusSocketException& e)
	//{
	//	std::cout << e.GetErrorMessage() << std::endl;
	//	return 1;
	//}

	//// Start to accept connections
	//size_t id = 1;
	//do {
	//	// create a pointer on empty tcp socket
	//	std::unique_ptr<DeusCore::TcpSocket> p_client = std::make_unique<DeusCore::TcpSocket>();
	//	try {
	//		do {
	//		} while (!listener.Accept(*p_client)); // timeout is in the accept function
	//	}
	//	catch (const DeusCore::DeusSocketException& e)
	//	{
	//		std::cout << e.GetErrorMessage() << std::endl;
	//		return 1;
	//	}

	//	std::cout << "[Server " << std::setw(3) << "" << "] Connection accepted !" << std::endl;
	//	
	//	// we want to transfert the ownership of our TCP socket to our DeusClient instance
	//	// which handle the communication behavior for TCP (and UDP too in the futur)
	//	clientsConnections.insert(
	//		std::make_pair(
	//			id, 
	//			std::unique_ptr<DeusServer::DeusClient>(new DeusServer::DeusClient(id, std::move(p_client)))));
	//	
	//	// TODO : subscribe to OnReceiveMessage Event

	//	/*
	//	// create deleguate functions
	//	DeusServer::DeusEventDeleguate messageTcpReceivedDeleguate = fastdelegate::MakeDelegate(this, &DeusServer::DeusClient::ManageTcpMessageReceivedEvent);
	//	DeusServer::DeusEventDeleguate disconnectTcpDeleguate = fastdelegate::MakeDelegate(this, &DeusServer::DeusClient::ManageTcpDisconnectedEvent);

	//	// register deleguates with connection
	//	m_tcpConnection.AddListener(messageTcpReceivedDeleguate, DeusConnection::DeusConnectionEventsType::OnMessageReceived);
	//	m_tcpConnection.AddListener(disconnectTcpDeleguate, DeusConnection::DeusConnectionEventsType::Disconnected);
	//	*/


	//	//clientsConnections[id]->AddListener()

	//	id++;
	//} while (wantToContinue);

	//listener.SocketShutdown();
	//listener.SocketClose();

	DeusServer::WorldServer server;
	server.Run();
	DeusCore::Logger::Instance()->Log("MAIN", "closing server");
	return 0;
}