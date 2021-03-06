// WorldServer.cpp 
//

#include "DeusCore/Logger.h"
#include "WorldNetworkServer.h"
#include "DeusCore/ResourcesHandler.h"


int main()
{
	try {
		DeusCore::ResourcesHandler::Instance(); // Init resources handler
	}
	catch (std::exception& e)
	{
		DeusCore::ResourcesHandler::Free();
		DeusCore::Logger::Instance()->Log("MAIN", "Closing server");
		DeusCore::Logger::Free();
		return 0;
	}

	// Initialize WSA
	WSADATA m_wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &m_wsaData);
	if (iResult != 0) {
		return 1;
	}

	std::string adress = "";
	std::string port = "";

	// INIT SERV INFOS THEN LAUNCH SERV
	std::cout << "Server adress (127.0.0.1 if leaved blank): ";
	std::cout.flush();
	std::getline(std::cin, adress);

	std::cout << "Server port   (27015 if leaved blank): " << std::endl;
	std::cout.flush();
	std::getline(std::cin, port);

	if (adress.empty())
		adress = "127.0.0.1";

	if (port.empty())
		port = "27015";

	DeusServer::WorldNetworkServer server;
	server.SetConnectInfo(adress, port);

	DeusCore::Logger::Instance()->Log("MAIN", "Starting server : " + adress + "@" + port);
	server.Start(0);
	server.WaitForStop();// blocking

	// STOP AND DELETE 
	DeusCore::ResourcesHandler::Free();

	DeusCore::Logger::Instance()->Log("MAIN", "Closing server");
	DeusCore::Logger::Free();

	WSACleanup();
	return 0;
}