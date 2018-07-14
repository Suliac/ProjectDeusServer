#include "PacketTest.h"

namespace DeusNetwork
{
	PacketTest::PacketTest() : Packet(Packet::EMessageType::MessageTest)
	{
	}


	PacketTest::~PacketTest()
	{
	}

	void PacketTest::OnDeserialize(Buffer512 & buffer)
	{
		// get the size of the string
		size_t dataSize;
		DeserializeData(buffer, dataSize);

		// get the string
		m_message.clear();
		std::vector<unsigned char> tmp = buffer.Select(dataSize);
		m_message.assign((char *)tmp.data());
	}

	void PacketTest::OnSerialize(Buffer512 & buffer) const
	{
		size_t dataSize = m_message.size() + 1; // +1 to add the \0 of string
		SerializeData(buffer, dataSize); // SerializeData only for primitive

		//  then we add the string 
		const char* myCurrentText = m_message.c_str();
		for (size_t i = 0; i < dataSize; i++)
		{
			SerializeData(buffer, *myCurrentText);
			myCurrentText++;
		}
	}
}
