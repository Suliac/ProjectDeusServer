#include "PacketAck.h"

namespace DeusCore
{
	PacketAck::PacketAck() : Packet(Packet::EMessageType::Ack)
	{
	}

	PacketAck::~PacketAck()
	{
	}

	void PacketAck::OnDeserialize(Buffer512 & buffer)
	{
		DeserializeData(buffer, m_packetToAck);
	}

	void PacketAck::OnSerialize(Buffer512 & buffer) const
	{
		SerializeData(buffer, m_packetToAck);
	}

	uint16_t PacketAck::EstimateCurrentSerializedSize() const
	{
		return sizeof(m_packetToAck);
	}
}
