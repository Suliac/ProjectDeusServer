#pragma once

#include "TcpSocket.h"


namespace DeusNetwork
{
	class TcpListener : public Socket
	{
	public:
		TcpListener();
		~TcpListener();

		// Init the listener with local ip adress & local port
		void Init(const std::string& ipAdress, const std::string& port, const bool setNonBlocking = false) ;

		// Bind the socket to local information and start listening for connections
		void Start();

		// Accept next connection and fill the TcpSocket object
		bool Accept(TcpSocket& socket) const;

		// TODO : Pending() function which return true or false if there is any pending connection
	};
}

