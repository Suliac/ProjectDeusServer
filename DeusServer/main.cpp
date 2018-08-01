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
	
	DeusCore::Logger::Instance()->Log("MAIN", "Starting server");

	DeusServer::WorldNetworkServer server;
	server.SetConnectInfo("127.0.0.1", "27015");
	server.Start(0);
	server.WaitForStop();// blocking

	DeusCore::Logger::Instance()->Log("MAIN", "Closing server");
	DeusCore::Logger::Free();

	WSACleanup();
	return 0;
}