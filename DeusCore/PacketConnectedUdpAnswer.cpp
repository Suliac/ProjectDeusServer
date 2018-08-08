#include "PacketConnectedUdpAnswer.h"

namespace DeusCore
{

	PacketConnectedUdpAnswer::PacketConnectedUdpAnswer()
		: Packet(Packet::EMessageType::ConnectedUdpAnswer)
	{
	}

	PacketConnectedUdpAnswer::~PacketConnectedUdpAnswer()
	{
	}

	void PacketConnectedUdpAnswer::OnDeserialize(Buffer512 & buffer)
	{
		// get the size of the string
		uint8_t dataSize;
		DeserializeData(buffer, dataSize);

		// get the string
		m_playerNickname.clear();
		std::vector<unsigned char> tmp = buffer.Select(dataSize);

		m_playerNickname.assign((char *)tmp.data());

	}

	void PacketConnectedUdpAnswer::OnSerialize(Buffer512 & buffer) const
	{
		uint8_t dataSize = m_playerNickname.size() + 1; // +1 to add the \0 of string
		SerializeData(buffer, dataSize); // SerializeData only for primitive

										 //  then we add the string 
		const char* myCurrentText = m_playerNickname.c_str();
		for (size_t i = 0; i < dataSize; i++)
		{
			SerializeData(buffer, *myCurrentText);
			myCurrentText++;
		}

	}

	uint16_t PacketConnectedUdpAnswer::EstimateCurrentSerializedSize() const
	{
		// 1 PacketClientConnected uses :
		// - 4 byte						: to save an unsigned int for the length of the next string
		// - m_message.size()+1 bytes	: to save the string ip address ('+1' is for the \0)
		return uint16_t(sizeof(uint8_t) + (m_playerNickname.size() + 1));
	}
}

