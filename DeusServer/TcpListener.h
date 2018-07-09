#ifndef _TCPLISTENER
#define _TCPLISTENER

#include "Socket.h"

namespace DeusNetwork
{
	class TcpListener : public Socket
	{
	public:
		TcpListener();
		~TcpListener();

		// Methods
		int Init(const char* ipAdress, const char* port);
		int Start();
		const SOCKET Accept();

		// TODO : Pending() function which return true or false if there is any pending connection
	};
}
#endif //_TCPLISTENER

