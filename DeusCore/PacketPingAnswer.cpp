#include "PacketPingAnswer.h"

namespace DeusCore
{
	PacketPingAnswer::PacketPingAnswer()
		: Packet(Packet::EMessageType::PingAnswer)
	{
	}
	
	PacketPingAnswer::~PacketPingAnswer()
	{
	}

	void PacketPingAnswer::OnDeserialize(Buffer512 & buffer)
	{
		DeserializeData(buffer, m_localTimestamp);
	}

	void PacketPingAnswer::OnSerialize(Buffer512 & buffer) const
	{
		SerializeData(buffer, m_localTimestamp);
	}

	uint16_t PacketPingAnswer::EstimateCurrentSerializedSize() const
	{
		return uint16_t(sizeof(m_localTimestamp));
	}
}
