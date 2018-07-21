#include "TcpSocket.h"
#include "Logger.h"
#include "DeusSocketException.h"

#include <WS2tcpip.h>

namespace DeusCore
{
	TcpSocket::TcpSocket() : Socket("TcpSocket")
	{
	}

	TcpSocket::~TcpSocket()
	{
	}

	void TcpSocket::TCPConnect(const SOCKET socket, const bool setNonBlocking)
	{
		assert(socket != INVALID_SOCKET);

		m_handler = socket;
		// We can set our socket non blocking
		if (setNonBlocking)
			SetNonBlocking();
	}

	bool TcpSocket::TCPConnect(const std::string& ipAdress, const std::string& port, const bool setNonBlocking)
	{
		// 1 - Init socket : get ipadress etc
		SocketInit(AF_INET, SOCK_STREAM, IPPROTO_TCP, AI_PASSIVE, ipAdress, port);

		// 2 - Create the socket
		SocketCreate();
		
		if (m_distantInfos == nullptr)
		{
			SocketClose();
			throw DeusSocketException("Impossible to retrieve connection informations");
		}
		
		// 3 - Connect (with the infos we got during the init of the socket)
		int result = connect(m_handler, m_distantInfos->ai_addr, (int)m_distantInfos->ai_addrlen);
		if (result == SOCKET_ERROR) {
			SocketClose();
			throw DeusSocketException("TCP Connection for [" + ipAdress + "@" + port + "] failed");
		}

		// We can set our socket non blocking when connection succeed
		if (setNonBlocking)
			SetNonBlocking();		

		// we don't need those informations anymore
		freeaddrinfo(m_distantInfos);

		if (m_handler == INVALID_SOCKET) {
			SocketClose();
			throw DeusSocketException("Unable to connect to server!");
		}

		return true;
	}

	/*************************************/
	/*                SEND               */
	/*************************************/

	bool TcpSocket::TCPSend(const Packet& paquet, int& byteSent)
	{
		Buffer512 buffer;
		Packet::Serialize(buffer, paquet);

		bool sendSuccess = TCPSend(buffer, byteSent);
		DeusCore::Logger::Instance()->Log("Client", "Bytes sent: "+std::to_string(byteSent));
		//std::cout << "Bytes sent: " << byteSent << std::endl;
		return sendSuccess;
	}

	bool TcpSocket::TCPSend(const Buffer512& buffer, int& byteSent)
	{
		return TCPSend((const char*)buffer.Data(), buffer.GetIndex(), byteSent);
	}

	bool TcpSocket::TCPSend(const char * data, size_t size, int& byteSent)
	{
		if (!CheckSocketStates(true, false)) // Check if socket is writable
			return false;

		byteSent = send(m_handler, data, size, 0);
		if (byteSent == SOCKET_ERROR) {
			std::string message = "send failed with error: " + std::to_string(WSAGetLastError());
			SocketClose();
			throw DeusSocketException(message);
		}

		return true;
	}

	/*************************************/
	/*             RECEIVED              */
	/*************************************/

	bool TcpSocket::TCPRecv(std::unique_ptr<Packet>& p_packetReceived, int& byteRecv)
	{
		p_packetReceived.reset();
		Buffer512 buffer;

		if (!TCPRecv(buffer, byteRecv))
			return false;

		if (byteRecv > 0)
		{
			DeusCore::Logger::Instance()->Log("Client", "Bytes received: " + std::to_string(byteRecv));
			//std::cout << "Bytes received: " << byteRecv << std::endl;
			p_packetReceived = Packet::Deserialize(buffer);
			DeusCore::Logger::Instance()->Log("Client", "Deserialized size: " + std::to_string(buffer.GetIndex()));
			//std::cout << "Deserialized size: " << buffer.GetIndex() << std::endl;
		}

		return true;
	}

	bool TcpSocket::TCPRecv(Buffer512& buffer, int& byteRecv)
	{
		char tmpBuffer[SIZE_BUFFER];

		if (!TCPRecv(tmpBuffer, SIZE_BUFFER, byteRecv))
			return false; // nothing to read ! 

		buffer.Set((const unsigned char*)tmpBuffer, byteRecv);
		return true;
	}

	bool TcpSocket::TCPRecv(char * data, size_t size, int & byteRecv)
	{
		if (!CheckSocketStates(false, true)) // Check if socket is readable
			return false;

		byteRecv = recv(m_handler, data, size, 0); // read socket's datas
		if (byteRecv < 0) // if there error, throw a DeusSocketException
		{
			std::string message = "recv failed with error: " + std::to_string(WSAGetLastError());
			SocketClose();
			throw DeusSocketException(message);
		}

		return true;
	}

}


