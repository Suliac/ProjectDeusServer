#include "Packets.h"
#include "DeusSerializationException.h"
namespace DeusCore
{
	Packet::Packet(Packet::EMessageType messageType)
	{
		m_type = messageType;
		m_uniqueId = reinterpret_cast<uint32_t>(this);
	}

	Packet::~Packet()
	{
	}

	std::unique_ptr<Packet> Packet::Deserialize(Buffer512 &buffer, const size_t bufferIndexOffset)
	{
		PacketUPtr p_packetDeserialized = nullptr;
		buffer.SetIndex(bufferIndexOffset);

		// Deserialize Header
		// Header content :
		// - 4 bytes	: unique identifier
		// - 1 byte		: packet type
		// - 2 bytes	: serialized size of packet (in byte) 

		// Serialize type
		uint32_t uniqueId;
		Packet::DeserializeData(buffer, uniqueId);

		// Type deserialization
		uint8_t type;
		Packet::DeserializeData(buffer, type);

		// Serialized size of the message to assert
		uint16_t serializedSize;
		Packet::DeserializeData(buffer, serializedSize);

		// TODO How to check before that id or serialized size arn't corrupted ?

		// NB : size is the total size -> size=512 for Buffer<512> 
		if (buffer.GetIndex() + serializedSize > buffer.size)
			return p_packetDeserialized; // cannot deserialize because we don't have enough datas

		// Specific packet deserialzation
		switch (type)
		{
		case Packet::Test:
			p_packetDeserialized = std::make_unique<PacketTest>();
			break;
		case Packet::Ack:
			p_packetDeserialized = std::make_unique<PacketAck>();
			break;
		case Packet::CreateGameRequest:
			p_packetDeserialized = std::make_unique<PacketCreateGameRequest>();
			break;
		case Packet::CreateGameAnswer:
			p_packetDeserialized = std::make_unique<PacketCreateGameAnswer>();
			break;
		case Packet::JoinGameRequest:
			p_packetDeserialized = std::make_unique<PacketJoinGameRequest>();
			break;
		case Packet::JoinGameAnswer:
			p_packetDeserialized = std::make_unique<PacketJoinGameAnswer>();
			break;
		case Packet::GetGameRequest:
			p_packetDeserialized = std::make_unique<PacketGetGamesRequest>();
			break;
		case Packet::GetGameAnswer:
			p_packetDeserialized = std::make_unique<PacketGetGamesAnswer>();
			break;
		case Packet::LeaveGameRequest:
			p_packetDeserialized = std::make_unique<PacketLeaveGameRequest>();
			break;
		case Packet::LeaveGameAnswer:
			p_packetDeserialized = std::make_unique<PacketLeaveGameAnswer>();
			break;
		default:
			throw DeusSerializationException("Impossible to deserialize object : unknown message type");
		}

		assert(p_packetDeserialized);

		// set the id of our packet
		p_packetDeserialized->SetId(uniqueId);

		// set the serializedsize in our packet
		p_packetDeserialized->SetSerializedSize(serializedSize);

		// Specific packet-type deserialzation
		p_packetDeserialized->OnDeserialize(buffer);

		// return true when we successfully deserialize the packet
		return p_packetDeserialized;
	}

	void Packet::Serialize(Buffer512 & buffer, const Packet & paquet, const size_t bufferIndexOffset)
	{
		buffer.SetIndex(bufferIndexOffset);

		// Serialize Header
		// Header content :
		// - 4 bytes	: unique identifier
		// - 1 byte		: packet type
		// - 2 bytes	: serialized size of packet (in byte) 

		// Serialize type
		paquet.SerializeData(buffer, paquet.GetId());

		// Serialize type
		paquet.SerializeData(buffer, paquet.GetType());

		// serialize size
		uint16_t serializedSize = paquet.EstimateCurrentSerializedSize() + paquet.HEADER_SIZE;
		paquet.SerializeData(buffer, serializedSize);

		// Specific packet-type serialzation
		paquet.OnSerialize(buffer);
	}
}
