#include "DeusTcpConnection.h"
#define NOMINMAX

namespace DeusServer
{
	DeusTcpConnection::DeusTcpConnection()
	{
		allByteReceivedBuffer.reserve(DEFAULT_DEUSCLIENT_BUFFER_SIZE);
	}

	DeusTcpConnection::~DeusTcpConnection()
	{
		m_cancellationRequested = true;
		m_communicationThread.join();
		std::cout << "end thread" << std::endl;
	}

	void DeusTcpConnection::Init(std::unique_ptr<DeusNetwork::TcpSocket> communicationSocket)
	{
		m_clientTCPSocket = std::move(communicationSocket); // transfert ownership

		// init thread only here
		//m_communicationThread = std::thread([this] { ThreadSendAndReceive(); });
		//std::cout << "end thread init" << std::endl;
	}

	void DeusTcpConnection::ThreadSendAndReceive()
	{
		std::cout << "start thread" << std::endl;
		while (!m_cancellationRequested)
		{
			///////////////////////////////////////////////////
			//       1- We first try to send our message     //
			///////////////////////////////////////////////////

			std::unique_ptr<DeusNetwork::Packet> p_toSendPacket = nullptr;
			while (TryTakePacket(p_toSendPacket))
			{
				// reset buffer & counter
				sentByteCount = 0;
				writeBuffer.SetIndex(0); 

				// NB : we send packet per packet
				if (p_toSendPacket)
				{
					// Serialize our packet
					DeusNetwork::Packet::Serialize(writeBuffer, *(p_toSendPacket.get()));

					// Send our serialized packet
					m_clientTCPSocket->TCPSend(*(p_toSendPacket), sentByteCount);
				}
			}

			///////////////////////////////////////////////////
			// 2- Then if there is data to receive, get them //
			///////////////////////////////////////////////////
			if (m_clientTCPSocket->DataAvailable())
			{
				///////////////////////////////////////////////////////////
				// 2.1 Get datas from the socket and put them into a buffer
				do {
					// reset buffer & counter
					readedByteCount = 0;
					readBuffer.SetIndex(0); 

					// Receive information in the readBuffer
					m_clientTCPSocket->TCPRecv(readBuffer, readedByteCount);

					// insert data we get in our general buffer
					allByteReceivedBuffer.insert(allByteReceivedBuffer.end(), readBuffer.Data(), readBuffer.Data() + readedByteCount);

					// we continue to read while there is data left and we already fill our previous buffer
				} while (m_clientTCPSocket->DataAvailable() && readedByteCount == readBuffer.size);

				///////////////////////////////////////////////////////////
				// 2.2 Serialize data from socket until our buffer is empty
				while (allByteReceivedBuffer.size() > 0)
				{
					// buffer max size should not be higher than max network buffer (= deserializeBuffer.size)
					int maxBufferSize = (int)(allByteReceivedBuffer.size()) > deserializeBuffer.size ? deserializeBuffer.size : ((int)(allByteReceivedBuffer.size()));
					
					deserializeBuffer.SetIndex(0);
					deserializeBuffer.Set(((unsigned char*)allByteReceivedBuffer.data()), maxBufferSize);
					
					assert(allByteReceivedBuffer.size() > 3); // headers need at least 3 bytes

					DeusNetwork::PacketUPtr p_packetDeserialized = DeusNetwork::Packet::Deserialize(deserializeBuffer);
					if (p_packetDeserialized)
					{
						// we can now delete the byte in our buffer corresponding to our packet serializedSize
						allByteReceivedBuffer.erase(allByteReceivedBuffer.begin(), allByteReceivedBuffer.begin() + p_packetDeserialized->GetSerializedSize());
					
						// TODO : raise event "OnMessageReceived"
					}
					else 
						break; // we cannot deserialize data for now, just continue
					
				}
			}
		}
	}
}
