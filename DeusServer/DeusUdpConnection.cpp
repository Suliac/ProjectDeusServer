#include "DeusUdpConnection.h"

#include "DeusCore/Logger.h"
#include "DeusCore/Packets.h"
#include "DeusCore/EventManagerHandler.h"

#include "DeusCore/DeusException.h"
#include "DeusCore/DeusSerializationException.h"
#include "DeusCore/DeusSocketException.h"

#include <sstream>
#include <iostream>
namespace DeusServer
{
	DeusUdpConnection::DeusUdpConnection(int id) : DeusConnection(id)
	{
		m_idsAcknoledged = std::vector<uint32_t>(256, 0);
		m_indexForIdsAcknoledged = 0;
	}

	//---------------------------------------------------------------------------------
	DeusUdpConnection::~DeusUdpConnection()
	{
		m_cancellationRequested = true;

		if (m_communicationThread.joinable())
			m_communicationThread.join();

		DeusCore::Logger::Instance()->Log("UDP Client " + std::to_string(m_id), "Delete UDP connection");
	}

	//---------------------------------------------------------------------------------
	void DeusUdpConnection::Init(const std::string& ipAddr, const std::string& port)
	{
		m_clientUDPSocket = std::make_unique<DeusCore::UdpSocket>(DeusCore::UdpSocket());

		std::stringstream sPort(port);
		uint32_t numberPort = 0;
		sPort >> numberPort;

		numberPort += m_id + DECAL_PORT_UDP;

		m_clientUDPSocket->Init(ipAddr, std::to_string(numberPort), false, false);

		// init thread only here
		m_communicationThread = std::thread([this] { ThreadSendAndReceive(); });
	}

	//---------------------------------------------------------------------------------
	// Threaded function
	//---------------------------------------------------------------------------------
	void DeusUdpConnection::ThreadSendAndReceive()
	{
		assert(m_clientUDPSocket);
		assert(!m_clientUDPSocket->IsClosed());

		while (!m_cancellationRequested)
		{
			try
			{

				m_packetQueueLock.lock();
				bool dataToRecv = m_clientUDPSocket->DataAvailable(0, TIMEOUT_US);
				if (dataToRecv || !m_packetsToSend.empty())
				{
					///////////////////////////////////////////////////
					//       1- We first try to send our messages    //
					///////////////////////////////////////////////////
					std::shared_ptr<DeusCore::Packet> p_toSendPacket = nullptr;

					while (TryTakePacket(p_toSendPacket))
					{
						// reset buffer & counter
						sentByteCount = 0;
						//DeusCore::Logger::Instance()->Log("UDP Client " + std::to_string(m_id), "Take packet");

						// NB : we send packet per packet
						if (p_toSendPacket)
						{
							if (m_udpConnectionInitialized)
							{
								// Send our serialized packet
								//if (p_toSendPacket->GetType() != DeusCore::Packet::Ack)
									//DeusCore::Logger::Instance()->Log("UDP Client " + std::to_string(m_id), "Send message id: " + std::to_string(p_toSendPacket->GetId()));

								m_clientUDPSocket->Send(*(p_toSendPacket), sentByteCount);
							}
							else {
								DeusCore::Logger::Instance()->Log("UDP Client", "Want to send message but udp connection isn't initialized");
							}
							// then re-put our packet into packet to send queue
							// in case of the packet isn't received by the client
							// and need to be resend. We don't requeue ACK
							if (p_toSendPacket->GetType() != DeusCore::Packet::Ack &&
								p_toSendPacket->GetType() != DeusCore::Packet::PingRequest &&
								p_toSendPacket->GetType() != DeusCore::Packet::PingAnswer)
							{
								//m_packetQueueLock.lock();
								m_packetsToRequeue.push_back(std::make_pair(GetTickCount(), p_toSendPacket));
								//m_packetQueueLock.unlock();
							}
						}
					}


					//m_packetQueueLock.lock();
					// Requeue the packet to resend
					while (!m_packetsToRequeue.empty())
					{
						m_packetsToSend.push_back(m_packetsToRequeue.front());
						m_packetsToRequeue.pop_front();
					}
					//m_packetQueueLock.unlock();


					///////////////////////////////////////////////////
					// 2- Then if there is data to receive, get them //
					///////////////////////////////////////////////////
					if (dataToRecv)
					{
						uint32_t recvTimeStamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
						///////////////////////////////////////////////////////////
						// 2.1 Get datas from the socket and put them into a buffer
						do {
							// reset buffer & counter
							readedByteCount = 0;
							readBuffer.SetIndex(0);

							// Receive information in the readBuffer
							if (m_clientUDPSocket->Recv(readBuffer, readedByteCount))
							{
								if (readedByteCount == 0)
								{
									DeusCore::Logger::Instance()->Log("UDP Client " + std::to_string(m_id), "Want to stop communication");
									m_cancellationRequested = true;
								}
								else {
									// insert data we get in our general buffer
									allByteReceivedBuffer.insert(allByteReceivedBuffer.end(), readBuffer.Data(), readBuffer.Data() + readedByteCount);
								}

							}

							// we continue to read while there is data left and we already fill our previous buffer
						} while (m_clientUDPSocket->DataAvailable() && readedByteCount == readBuffer.size);

						///////////////////////////////////////////////////////////
						// 2.2 Deserialize data from socket until our buffer is empty
						while (allByteReceivedBuffer.size() > 0)
						{
							// buffer max size should not be higher than max network buffer (= deserializeBuffer.size)
							int maxBufferSize = (int)(allByteReceivedBuffer.size()) > deserializeBuffer.size ? deserializeBuffer.size : ((int)(allByteReceivedBuffer.size()));

							deserializeBuffer.SetIndex(0);
							deserializeBuffer.Set(((unsigned char*)allByteReceivedBuffer.data()), maxBufferSize);

							assert(allByteReceivedBuffer.size() >= DeusCore::Packet::HEADER_SIZE); // headers need at least 3 bytes

							DeusCore::PacketSPtr p_packetDeserialized = std::move(DeusCore::Packet::Deserialize(deserializeBuffer));

							if (p_packetDeserialized)
							{
								// we can now delete the byte in our buffer corresponding to our packet serializedSize
								allByteReceivedBuffer.erase(allByteReceivedBuffer.begin(), allByteReceivedBuffer.begin() + p_packetDeserialized->GetSerializedSize());

								// the only case where a packet isn't added to the event queue is for 
								// our custom mini ACK system or ping answer, which is reliable but unordered
								if (p_packetDeserialized->GetType() == DeusCore::Packet::EMessageType::Ack)
								{
									// if the message is an ACK, add the id ACKed to our vector
									uint32_t idAck = ((DeusCore::PacketAck*)p_packetDeserialized.get())->GetPacketToAck();

									// we check that we don't already have this ack
									if (std::find(m_idsAcknoledged.begin(), m_idsAcknoledged.end(), idAck) == m_idsAcknoledged.end())
									{
										//m_packetQueueLock.lock(); // <--------- LOCK
										std::list<std::pair<long, DeusCore::PacketSPtr>>::iterator findIt;
										for (findIt = m_packetsToSend.begin(); findIt != m_packetsToSend.end(); findIt++)
										{
											if (findIt->second->GetId() == idAck)
											{
												//DeusCore::Logger::Instance()->Log("UDP Client " + std::to_string(m_id), "Received ACK for Message id: " + std::to_string(idAck));
												// idAck isn't in our vector, we can add it
												m_idsAcknoledged[m_indexForIdsAcknoledged] = idAck;
												m_indexForIdsAcknoledged++;

												// reset index if too high
												if (m_indexForIdsAcknoledged > 255)
													m_indexForIdsAcknoledged = 0;

												// Then finally we erase our packet acked
												m_packetsToSend.erase(findIt);
												break;
											}
										}
										//m_packetQueueLock.unlock(); // <--------- UNLOCK

									}
									// else, ack id already in our list, nothing to do
								}
								else {
									// if our packet isn't an ACK, we send some ACKs back for the packet just received
									for (size_t i = 0; i < ACK_NUMBER; i++)
									{
										DeusCore::PacketUPtr p_ackPacket = std::unique_ptr<DeusCore::PacketAck>(new DeusCore::PacketAck());
										((DeusCore::PacketAck*)p_ackPacket.get())->SetPacketToAck(p_packetDeserialized->GetId());
										AddPacketToQueue(std::move(p_ackPacket));
									}

									if (p_packetDeserialized->GetType() == DeusCore::Packet::EMessageType::PingRequest)
									{
										uint32_t idPing = ((DeusCore::PacketPingRequest*)p_packetDeserialized.get())->GetId();
										std::shared_ptr<DeusCore::PacketPingAnswer> p_answerPing = std::shared_ptr<DeusCore::PacketPingAnswer>(new DeusCore::PacketPingAnswer());
										p_answerPing->SetPacketRecvId(idPing);
										//AddPacketToQueue(p_answerPing);
										m_clientUDPSocket->Send(*(p_answerPing), sentByteCount);
									}
									else if (p_packetDeserialized->GetType() == DeusCore::Packet::EMessageType::ClockSyncRequest)
									{
										std::shared_ptr<DeusCore::PacketSyncClockAnswer> p_answerSync = std::shared_ptr<DeusCore::PacketSyncClockAnswer>(new DeusCore::PacketSyncClockAnswer());
										p_answerSync->SetServerTimeStamp(recvTimeStamp);

										m_clientUDPSocket->Send(*(p_answerSync), sentByteCount);
									}
									else
									{
										// Finally enqueue our event
										//DeusCore::Logger::Instance()->Log("UDP Client " + std::to_string(m_id), "Received Message id: " + std::to_string(p_packetDeserialized->GetId()));
										DeusCore::EventManagerHandler::Instance()->QueueEvent(m_gameId, m_id, p_packetDeserialized);
									}
								}
							}
							else
								break; // we cannot deserialize data for now, just continue

						} // end while(allByteReceivedBuffer.size() > 0)
					} // end if (dataToRecv)

					m_packetQueueLock.unlock();
					std::this_thread::sleep_for(std::chrono::microseconds(100));
				} // end if (dataToRecv || !m_packetsToSend.empty())
				else {
					m_packetQueueLock.unlock();
					// if there isn't any data to send or receive, just wait 
					std::this_thread::sleep_for(std::chrono::microseconds(TIMEOUT_US));
				}
			}
			catch (const DeusCore::DeusSerializationException& e)
			{
				m_packetQueueLock.unlock();
				DeusCore::Logger::Instance()->Log("UDP Client " + std::to_string(m_id), "Not blocking error : " + e.GetErrorMessage());
			}
			catch (const DeusCore::DeusSocketException& e)
			{
				m_packetQueueLock.unlock();
				DeusCore::Logger::Instance()->Log("UDP Client " + std::to_string(m_id), "Connection aborted. Error : " + e.GetErrorMessage());
				m_cancellationRequested = true;
			}
		}

		DeusCore::Logger::Instance()->Log("Client " + std::to_string(m_id), "End thread");
	}

	//---------------------------------------------------------------------------------
	bool DeusUdpConnection::TryTakePacket(DeusCore::PacketSPtr & p_packet)
	{
		//
		//		 +------------------+
		//		 |				    |
		//		 |  m_packetsToSend <----+
		//		 |				    |	 |
		//		 +-+----------------+	 |
		// 		   |					 |
		// ACKED<--+----->SEND-----+	 |
		// 		   |			   |	 |
		//		   +->CAN'T SEND---+ 	 |
		//		                   | 	 |
		//		+------------------v-+	 |
		//		|				     |	 |
		//	 +--+ m_packetsToRequeue |   |
		//	 |	|				     |   |
		//	 |	+--------------------+   |
		//	 |							 |
		//	 +---------------------------+

		bool getPacket = false;

		// don't block the execution :
		// add packet is more important than try to take them
		//if (m_packetQueueLock.try_lock())
		//{

		while (!m_packetsToSend.empty() && !getPacket)
		{
			// We check that the next isn't one we already sent and we need to verify
			// if we recv an ack, if not we have to resend the packet
			if (m_packetsToSend.front().first == 0
				|| m_packetsToSend.front().first + PACKET_DELAY_CHECK_ACK_MS <= GetTickCount())
			{
				// we need to know if there is already an ACK for this packet
				if (std::find(m_idsAcknoledged.begin(), m_idsAcknoledged.end(), m_packetsToSend.front().second->GetId()) != m_idsAcknoledged.end())
				{
					// this packet is already acknowledged : just pop it 
					//DeusCore::Logger::Instance()->Log("UDP Client " + std::to_string(m_id), "Message Acked : " + std::to_string(m_packetsToSend.front().second->GetId()));
					m_packetsToSend.pop_front();

				}
				else {
					// This packet wasn't sent or didn't received ack
					p_packet = std::move(m_packetsToSend.front().second);
					m_packetsToSend.pop_front();
					getPacket = true;
				}
			}
			else {
				// if this is too early to send back the packet, just requeue it
				// at the end to not block potential new packets
				//DeusCore::Logger::Instance()->Log("UDP Client " + std::to_string(m_id), "Re-enqueue message : " + std::to_string(m_packetsToSend.front().second->GetId()));

				m_packetsToRequeue.push_back(m_packetsToSend.front());
				m_packetsToSend.pop_front();
			}
		}

		//m_packetQueueLock.unlock();
		//}

		return getPacket;
	}
}
