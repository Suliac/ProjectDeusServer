#include "PacketTest.h"

namespace DeusCore
{
	PacketTest::PacketTest() : Packet(Packet::EMessageType::Test)
	{
	}


	PacketTest::~PacketTest()
	{
	}

	void PacketTest::OnDeserialize(Buffer512 & buffer)
	{
		// get the size of the string
		uint16_t dataSize;
		DeserializeData(buffer, dataSize);

		// get the string
		m_message.clear();
		std::vector<unsigned char> tmp = buffer.Select(dataSize);

		m_message.assign((char *)tmp.data());

	}

	void PacketTest::OnSerialize(Buffer512 & buffer) const
	{
		uint16_t dataSize = m_message.size() + 1; // +1 to add the \0 of string
		SerializeData(buffer, dataSize); // SerializeData only for primitive

		//  then we add the string 
		const char* myCurrentText = m_message.c_str();
		for (size_t i = 0; i < dataSize; i++)
		{
			SerializeData(buffer, *myCurrentText);
			myCurrentText++;
		}
	}

	uint16_t PacketTest::EstimateCurrentSerializedSize() const
	{
		// 1 PacketTest packet uses :
		// - 1 byte						: to save an unsigned int for the length of the next string
		// - m_message.size()+1 bytes	: to save the string message ('+1' is for the \0)
		return sizeof(uint16_t) + (m_message.size() + 1);
	}
}
