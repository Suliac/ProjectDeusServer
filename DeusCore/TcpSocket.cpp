#include "TcpSocket.h"
#include "DeusSocketException.h"

#include <WS2tcpip.h>

namespace DeusNetwork
{
	TcpSocket::TcpSocket() : Socket()
	{
	}

	TcpSocket::~TcpSocket()
	{
		SocketClose();
	}

	void TcpSocket::TCPConnect(const SOCKET socket)
	{
		m_handler = socket;
		m_state = SocketState::SOCKET_READY;
	}

	void TcpSocket::TCPConnect(const std::string& ipAdress, const std::string& port)
	{
		try {
			SocketInit(AF_INET, SOCK_STREAM, IPPROTO_TCP, AI_PASSIVE, ipAdress, port);
			SocketCreate();
		}
		catch (DeusSocketException const e)
		{
			throw e;
		}

		if (m_distantInfos == nullptr)
			throw DeusSocketException("Impossible to retrieve connection informations");

		// Connect with the infos we got during the init of the socket
		int result = connect(m_handler, m_distantInfos->ai_addr, (int)m_distantInfos->ai_addrlen);
		if (result == SOCKET_ERROR) {
			closesocket(m_handler);
			m_handler = INVALID_SOCKET;
			throw DeusSocketException("TCP Connection for [" + ipAdress + "@" + port + "] failed");
		}

		// we don't need those informations anymore
		freeaddrinfo(m_distantInfos);

		if (m_handler == INVALID_SOCKET) {
			WSACleanup();
			throw DeusSocketException("Unable to connect to server!");
		}

		m_state = SocketState::SOCKET_READY;
	}

	void TcpSocket::TCPSend(const Packet& paquet, size_t& byteSent) const
	{
		Buffer512 buffer;
		Packet::Serialize(buffer, paquet);

		try
		{
			TCPSend(buffer, byteSent);
			std::cout << "Bytes sent: " << byteSent << std::endl;
		}
		catch (const DeusSocketException& ex)
		{
			throw ex;
		}
	}

	void TcpSocket::TCPSend(const Buffer512& buffer, size_t& byteSent) const
	{
		try
		{
			TCPSend((const char*)buffer.Data(), buffer.GetIndex(), byteSent);
		}
		catch (const DeusSocketException& ex)
		{
			throw ex;
		}
	}

	void TcpSocket::TCPSend(const char * data, std::size_t size, std::size_t & byteSent) const
	{
		if (m_state != SocketState::SOCKET_READY)
			throw DeusSocketException("Socket wasn't ready to send");

		byteSent = send(m_handler, data, size, 0);
		if (byteSent == SOCKET_ERROR) {
			std::string message = "send failed with error: " + std::to_string(SocketGetLastError());
			closesocket(m_handler);
			WSACleanup();
			throw DeusSocketException(message);
		}
	}

	void TcpSocket::TCPRecv(Buffer512& buffer, size_t& byteRecv) const
	{
		char tmpBuffer[SIZE_BUFFER];

		try
		{
			TCPRecv(tmpBuffer, SIZE_BUFFER, byteRecv);
		}
		catch (const DeusSocketException& ex)
		{
			throw ex;
		}

		buffer.Set((const unsigned char*)tmpBuffer, byteRecv);
	}

	std::unique_ptr<Packet> TcpSocket::TCPRecv(size_t & byteRecv) const
	{
		std::unique_ptr<Packet> p_packetReceived = nullptr;
		Buffer512 buffer;

		try
		{
			TCPRecv(buffer, byteRecv);

			if (byteRecv > 0)
			{
				std::cout << "Bytes received: " << byteRecv << std::endl;
				p_packetReceived = Packet::Deserialize(buffer);
				std::cout << "Deserialized size: " << buffer.GetIndex() << std::endl;
			}
		}
		catch (const DeusSocketException& ex)
		{
			throw ex;
		}

		return p_packetReceived;
	}

	void TcpSocket::TCPRecv(char * data, size_t size, size_t & byteRecv) const
	{
		byteRecv = recv(m_handler, data, size, 0); // récupérer les données réçue sur cette socket
		if (byteRecv < 0) // données reçues
		{
			std::string message = "recv failed with error: " + std::to_string(SocketGetLastError());
			closesocket(m_handler);
			WSACleanup();
			throw DeusSocketException(message);
		}
	}
}


