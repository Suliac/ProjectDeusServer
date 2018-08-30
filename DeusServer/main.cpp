// WorldServer.cpp 
//

#include "DeusCore/Logger.h"
#include "WorldNetworkServer.h"



int main()
{
	// Initialize WSA
	WSADATA m_wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &m_wsaData);
	if (iResult != 0) {
		return 1;
	}

	std::string adress = "";
	std::string port = "";

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

	DeusCore::Logger::Instance()->Log("MAIN", "Closing server");
	DeusCore::Logger::Free();

	WSACleanup();
	return 0;
}