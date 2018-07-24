#include "DeusTcpConnection.h"

#include "DeusCore/Packets.h"
#include "DeusCore/Logger.h"
#include "DeusCore/DeusException.h"
#include "DeusCore/DeusSerializationException.h"
#include "DeusCore/DeusSocketException.h"
#include "DeusCore/EventManagerHandler.h"

#include <chrono>
#define NOMINMAX

namespace DeusServer
{
	DeusTcpConnection::DeusTcpConnection(int id) : DeusConnection(id)
	{
		allByteReceivedBuffer.reserve(DEFAULT_DEUSCLIENT_BUFFER_SIZE);
	}

	DeusTcpConnection::~DeusTcpConnection()
	{
		m_cancellationRequested = true;

		if (m_communicationThread.joinable())
			m_communicationThread.join();

		DeusCore::Logger::Instance()->Log("Client " + std::to_string(m_id), "Delete connection");
	}

	void DeusTcpConnection::Init(std::unique_ptr<DeusCore::TcpSocket> communicationSocket)
	{
		m_clientTCPSocket = std::move(communicationSocket); // transfert ownership

		// init thread only here
		m_communicationThread = std::thread([this] { ThreadSendAndReceive(); });
	}

	//---------------------------------------------------------------------------------
	// Threaded function
	//---------------------------------------------------------------------------------
	void DeusTcpConnection::ThreadSendAndReceive()
	{
		DeusCore::Logger::Instance()->Log("Client " + std::to_string(m_id), "Start thread");

		while (!m_cancellationRequested)
		{
			try
			{
				// DeusCore::Logger::Instance()->Log("Client " + std::to_string(m_id), "Loop");
				bool dataToRecv = m_clientTCPSocket->DataAvailable(0, TIMEOUT_US);
				if (dataToRecv || !m_packetsToSend.empty())
				{
					///////////////////////////////////////////////////
					//       1- We first try to send our message     //
					///////////////////////////////////////////////////
					std::unique_ptr<DeusCore::Packet> p_toSendPacket = nullptr;
					while (TryTakePacket(p_toSendPacket))
					{
						// reset buffer & counter
						sentByteCount = 0;
						writeBuffer.SetIndex(0);

						// NB : we send packet per packet
						if (p_toSendPacket)
						{
							// Serialize our packet
							DeusCore::Packet::Serialize(writeBuffer, *(p_toSendPacket.get()));

							// Send our serialized packet
							m_clientTCPSocket->TCPSend(*(p_toSendPacket), sentByteCount);
						}
					}

					///////////////////////////////////////////////////
					// 2- Then if there is data to receive, get them //
					///////////////////////////////////////////////////
					if (dataToRecv)
					{
						///////////////////////////////////////////////////////////
						// 2.1 Get datas from the socket and put them into a buffer
						do {
							// reset buffer & counter
							readedByteCount = 0;
							readBuffer.SetIndex(0);

							// Receive information in the readBuffer
							if (m_clientTCPSocket->TCPRecv(readBuffer, readedByteCount))
							{
								if (readedByteCount == 0)
								{
									DeusCore::Logger::Instance()->Log("Client " + std::to_string(m_id), "Want to stop communication");
									//std::cout << "[Client " << std::setw(3) << m_id << "] " << std::endl;

									m_cancellationRequested = true;
								}
								else {
									// insert data we get in our general buffer
									allByteReceivedBuffer.insert(allByteReceivedBuffer.end(), readBuffer.Data(), readBuffer.Data() + readedByteCount);
								}

							}

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

							assert(allByteReceivedBuffer.size() >= 3); // headers need at least 3 bytes

							DeusCore::PacketSPtr p_packetDeserialized = std::move(DeusCore::Packet::Deserialize(deserializeBuffer));
							if (p_packetDeserialized)
							{
								// we can now delete the byte in our buffer corresponding to our packet serializedSize
								allByteReceivedBuffer.erase(allByteReceivedBuffer.begin(), allByteReceivedBuffer.begin() + p_packetDeserialized->GetSerializedSize());

								///////////////////////////////////////////////////////////
								// Queue event received
								DeusCore::EventManagerHandler::Instance()->QueueEvent(m_gameId, m_id, p_packetDeserialized);
							}
							else
								break; // we cannot deserialize data for now, just continue

						} // end while(allByteReceivedBuffer.size() > 0)
					} // end if (dataToRecv)
				} // end if (dataToRecv || !m_packetsToSend.empty())
				else {
					// if there isn't any data to send or receive, just wait 
					std::this_thread::sleep_for(std::chrono::microseconds(TIMEOUT_US));
				}
			}
			catch (const DeusCore::DeusSerializationException& e)
			{
				DeusCore::Logger::Instance()->Log("Client " + std::to_string(m_id), "Not blocking error : " + e.GetErrorMessage());
			}
			catch (const DeusCore::DeusSocketException& e)
			{
				DeusCore::Logger::Instance()->Log("Client " + std::to_string(m_id), "Connection aborted. Error : " + e.GetErrorMessage());
				m_cancellationRequested = true;
			}
		}

		///////////////////////////////////////////////////////////
		// Queue event disconnect
		DeusCore::PacketSPtr p_disconnectedEvent = std::shared_ptr<DeusCore::PacketClientDisconnect>(new DeusCore::PacketClientDisconnect());
		DeusCore::EventManagerHandler::Instance()->QueueEvent(m_gameId, m_id, p_disconnectedEvent);

		DeusCore::Logger::Instance()->Log("Client " + std::to_string(m_id), "End thread");
	}

	//---------------------------------------------------------------------------------
	// Threaded function
	//---------------------------------------------------------------------------------
	//void DeusTcpConnection::CheckRecvOrSend()
	//{
	//	if (m_dataToSendOrRecvLock.try_lock())
	//	{
	//		m_dataToSendOrRecv = m_clientTCPSocket->DataAvailable(0, 5000) || !m_packetsToSend.empty();
	//		m_dataToSendOrRecvLock.unlock();
	//		m_sendOrRecvBlocker.notify_one();
	//	}
	//	else {
	//		std::this_thread::sleep_for(std::chrono::microseconds(5000));
	//	}
	//}

}
