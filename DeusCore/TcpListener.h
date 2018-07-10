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
		void Init(std::string ipAdress, std::string port);

		// Bind the socket to local information and start listening for connections
		void Start();

		// Accept next connection and fill the TcpSocket object
		void Accept(TcpSocket& socket);

		// TODO : Pending() function which return true or false if there is any pending connection
	};
}

