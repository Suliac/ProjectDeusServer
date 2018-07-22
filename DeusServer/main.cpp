// WorldServer.cpp 
//

#include "DeusCore/Logger.h"
#include "WorldNetworkServer.h"

int main()
{
	DeusCore::Logger::Instance()->Log("MAIN", "Starting server");

	DeusServer::WorldNetworkServer server;
	server.SetConnectInfo("127.0.0.1", "27015");
	server.Run();

	DeusCore::Logger::Instance()->Log("MAIN", "Closing server");
	return 0;
}