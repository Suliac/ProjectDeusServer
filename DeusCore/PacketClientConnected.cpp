#include "PacketClientConnected.h"


namespace DeusCore
{
	
	PacketClientConnected::PacketClientConnected()
		: Packet(Packet::EMessageType::Connected)
	{
	}

	PacketClientConnected::~PacketClientConnected()
	{
	}

	void PacketClientConnected::OnDeserialize(Buffer512 & buffer)
	{
		// get the size of the string
		size_t dataSize;
		DeserializeData(buffer, dataSize);

		// get the string
		m_addrUdp.clear();
		std::vector<unsigned char> tmp = buffer.Select(dataSize);

		m_addrUdp.assign((char *)tmp.data());

		// then deserialize the port
		DeserializeData(buffer, m_portUdp);
	}

	void PacketClientConnected::OnSerialize(Buffer512 & buffer) const
	{
		size_t dataSize = m_addrUdp.size() + 1; // +1 to add the \0 of string
		SerializeData(buffer, dataSize); // SerializeData only for primitive

										 //  then we add the string 
		const char* myCurrentText = m_addrUdp.c_str();
		for (size_t i = 0; i < dataSize; i++)
		{
			SerializeData(buffer, *myCurrentText);
			myCurrentText++;
		}

		// then serialize the port
		SerializeData(buffer, m_portUdp);
	}

	uint16_t PacketClientConnected::EstimateCurrentSerializedSize() const
	{
		// 1 PacketClientConnected uses :
		// - 1 byte						: to save an unsigned int for the length of the next string
		// - m_message.size()+1 bytes	: to save the string ip address ('+1' is for the \0)
		// - 4 bytes					: to save the udp port
		return sizeof(size_t) + (m_addrUdp.size() + 1) + sizeof(m_portUdp);
	}
}
