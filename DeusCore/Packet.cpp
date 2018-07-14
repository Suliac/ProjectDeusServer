#include "Packet.h"
#include "PacketTest.h"
#include "DeusSerializationException.h"
namespace DeusNetwork
{
	Packet::Packet() : Packet(Packet::EMessageType::MessageTest)
	{
	}
	
	Packet::~Packet()
	{
	}

	std::unique_ptr<Packet> Packet::Deserialize(Buffer512 & buffer, const size_t bufferIndexOffset)
	{
		std::unique_ptr<Packet> p_packetDeserialized = nullptr;

		buffer.SetIndex(bufferIndexOffset);

		uint8_t id;
		Packet::DeserializeData(buffer, id);

		switch (id)
		{
		case Packet::MessageTest:
			p_packetDeserialized = std::unique_ptr<Packet>(new PacketTest());
			break;
		default:
			throw DeusSerializationException("Impossible to deserialize object : unknown message type");
			break;
		}

		if (p_packetDeserialized == nullptr)
			throw DeusSerializationException("Impossible to deserialize object : null pointer");

		p_packetDeserialized->OnDeserialize(buffer);
		return p_packetDeserialized;
	}

	void Packet::Serialize(Buffer512 & buffer, const Packet & paquet, const size_t bufferIndexOffset)
	{
		buffer.SetIndex(bufferIndexOffset);
		paquet.SerializeData(buffer, paquet.GetID());
		paquet.OnSerialize(buffer);
	}
}
