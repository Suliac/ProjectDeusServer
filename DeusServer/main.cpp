// WorldServer.cpp 
//

#include "DeusCore/Logger.h"
#include "WorldServer.h"

int main()
{
	DeusCore::Logger::Instance()->Log("MAIN", "Starting server");
	DeusServer::WorldServer server;
	server.Run();
	DeusCore::Logger::Instance()->Log("MAIN", "Closing server");
	return 0;
}