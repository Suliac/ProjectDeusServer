#include "DeusUdpListener.h"

#include "DeusCore/Logger.h"
#include "DeusCore/Packets.h"
#include "DeusCore/EventManagerHandler.h"
#include "DeusCore/DeusException.h"
#include "DeusCore/DeusSerializationException.h"
#include "DeusCore/DeusSocketException.h"
namespace DeusServer
{
	DeusUdpListener* DeusUdpListener::p_instance = nullptr;
	std::mutex DeusUdpListener::m_instanceLock;

	void DeleteAddrNfo(addrinfo* addr) {
		if (addr != nullptr)
			freeaddrinfo(addr);
	}

	/////////////////////////
	//     SINGLETON	   //
	/////////////////////////
	//---------------------------------------------------------------------------------
	DeusUdpListener* DeusUdpListener::Instance()
	{
		if (p_instance == nullptr) {
			m_instanceLock.lock();
			if (p_instance == nullptr) {
				DeusUdpListener* temp = new DeusUdpListener;
				p_instance = temp;
			}
			m_instanceLock.unlock();
		}

		return p_instance;
	}

	void DeusUdpListener::Free() {
		if (p_instance != nullptr) {
			m_instanceLock.lock();
			if (p_instance != nullptr) {
				delete p_instance;
			}
			m_instanceLock.unlock();
		}
	}

	DeusUdpListener::DeusUdpListener()
	{
		m_idsAcknoledged = std::vector<uint32_t>(256, 0);
		m_indexForIdsAcknoledged = 0;
	}

	DeusUdpListener::~DeusUdpListener()
	{
		m_cancellationRequested = true;

		if (m_communicationThread.joinable())
			m_communicationThread.join();
	}

	void DeusUdpListener::AddPacketToQueue(PacketUDPSPtr p_packet)
	{
		if (!p_packet)
			return;

		m_packetQueueLock.lock();
		//DeusCore::Logger::Instance()->Log("UDP Client " + std::to_string(m_id), "Push packet");
		m_packetsToSend.push_back(std::make_pair(0, p_packet));
		m_packetQueueLock.unlock();

		p_packet.reset(); // we don't have packet ownership anymore
	}

	void DeusUdpListener::NewClient(std::shared_ptr<DeusListeningUdpConnection> p_udpConnection, std::shared_ptr<addrinfo> p_addr)
	{
		clientsLock.lock();
		m_clientWithAdress.push_back(std::make_pair(p_addr, p_udpConnection));
		clientsLock.unlock();
	}

	std::shared_ptr<DeusListeningUdpConnection> DeusUdpListener::FindClientWithAddr(std::shared_ptr<addrinfo> p_addr)
	{
		clientsLock.lock();
		for (const auto& pair : m_clientWithAdress)
		{
			if(p_addr->ai_family != pair.first->ai_family)
				continue;

			 if (p_addr->ai_family == AF_INET) {
				 struct sockaddr_in *addrin = (sockaddr_in*)(p_addr->ai_addr);
				 struct sockaddr_in *memberin = (sockaddr_in*)(pair.first->ai_addr);

				 // addr & port matches
				 if (ntohl(addrin->sin_addr.s_addr) == ntohl(memberin->sin_addr.s_addr)
					 && ntohs(addrin->sin_port) == ntohs(memberin->sin_port))
				 {
					 std::shared_ptr<DeusListeningUdpConnection> p_udpConnection = pair.second;

					 clientsLock.unlock();
					 return p_udpConnection;
				 }
			}
			else if (p_addr->ai_family == AF_INET6) {
				struct sockaddr_in6 *addrin = (sockaddr_in6*)(p_addr->ai_addr);
				struct sockaddr_in6 *memberin = (sockaddr_in6*)(pair.first->ai_addr);

				int r = memcmp(addrin->sin6_addr.s6_addr, memberin->sin6_addr.s6_addr, sizeof(addrin->sin6_addr.s6_addr));
				if (r != 0) // addr don't match
					continue;

				if (ntohs(addrin->sin6_port) == ntohs(memberin->sin6_port)
					&& addrin->sin6_flowinfo == memberin->sin6_flowinfo
					&& addrin->sin6_scope_id == memberin->sin6_scope_id)
				{
					// match !
					std::shared_ptr<DeusListeningUdpConnection> p_udpConnection = pair.second;

					clientsLock.unlock();
					return p_udpConnection;
				}
			}
			else {
				assert(!"unknown sa_family");
			}

			return 0;
		}
		clientsLock.unlock();
		return nullptr;
	}

	//---------------------------------------------------------------------------------
	// Threaded function
	//---------------------------------------------------------------------------------
	void DeusUdpListener::ThreadSendAndReceive()
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
					PacketUDPSPtr p_toSendPacket = nullptr;

					while (TryTakePacket(p_toSendPacket))
					{
						// reset buffer & counter
						sentByteCount = 0;

						// NB : we send packet per packet
						if (p_toSendPacket)
						{
							// Send our serialized packet
							m_clientUDPSocket->SendTo(*(p_toSendPacket), sentByteCount);

							// then re-put our packet into packet to send queue
							// in case of the packet isn't received by the client
							// and need to be resend. We don't requeue ACK
							if (p_toSendPacket->GetType() != DeusCore::Packet::Ack)
							{
								m_packetsToRequeue.push_back(std::make_pair(GetTickCount(), p_toSendPacket));
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
						std::shared_ptr<addrinfo> p_addrReceiver = std::shared_ptr<addrinfo>(nullptr, DeleteAddrNfo);
						///////////////////////////////////////////////////////////
						// 2.1 Get datas from the socket and put them into a buffer
						do {
							// reset buffer & counter
							readedByteCount = 0;
							readBuffer.SetIndex(0);

							// Receive information in the readBuffer
							if (m_clientUDPSocket->RecvFrom(readBuffer, readedByteCount, p_addrReceiver))
							{
								if (readedByteCount == 0)
								{
									DeusCore::Logger::Instance()->Log("UDP Listener", "Want to stop communication");
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
								// our custom mini ACK system, which is reliable but unordered
								if (p_packetDeserialized->GetType() == DeusCore::Packet::EMessageType::Ack)
								{
									// if the message is an ACK, add the id ACKed to our vector
									uint32_t idAck = ((DeusCore::PacketAck*)p_packetDeserialized.get())->GetPacketToAck();

									// we check that we don't already have this ack
									if (std::find(m_idsAcknoledged.begin(), m_idsAcknoledged.end(), idAck) == m_idsAcknoledged.end())
									{
										//m_packetQueueLock.lock(); // <--------- LOCK
										UDPPackets::iterator findIt;
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
										
										PacketUDPSPtr p_UDPACKPacket = std::shared_ptr<DeusCore::PacketSendUDP>(new DeusCore::PacketSendUDP());
										p_UDPACKPacket->PacketToSend = std::move(p_ackPacket);
										p_UDPACKPacket->AddrInfos = p_addrReceiver;
										AddPacketToQueue(p_UDPACKPacket);
									}

									// Finally notify ou DeusUdpConnection for the right client that there is a new message
									//DeusCore::Logger::Instance()->Log("UDP Listener", "Received Message id: " + std::to_string(p_packetDeserialized->GetId()));
									std::shared_ptr<DeusListeningUdpConnection> matchingClient = FindClientWithAddr(p_addrReceiver);
									if (matchingClient)
										matchingClient->RecvPacket(p_packetDeserialized);
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
				DeusCore::Logger::Instance()->Log("UDP Listener", "Not blocking error : " + e.GetErrorMessage());
			}
			catch (const DeusCore::DeusSocketException& e)
			{
				m_packetQueueLock.unlock();
				DeusCore::Logger::Instance()->Log("UDP Listener", "Connection aborted. Error : " + e.GetErrorMessage());
				m_cancellationRequested = true;
			}
		}

		DeusCore::Logger::Instance()->Log("UDP Listener", "End thread");
	}

	//---------------------------------------------------------------------------------
	bool DeusUdpListener::TryTakePacket(PacketUDPSPtr& p_packet)
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
