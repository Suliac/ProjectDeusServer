#include "PacketJoinGameRequest.h"

namespace DeusCore
{
	PacketJoinGameRequest::PacketJoinGameRequest()
		: Packet(Packet::EMessageType::JoinGameRequest)
	{
	}

	PacketJoinGameRequest::~PacketJoinGameRequest()
	{
	}

	void PacketJoinGameRequest::OnDeserialize(Buffer512 & buffer)
	{
		DeserializeData(buffer, m_gameId);
	}
	
	void PacketJoinGameRequest::OnSerialize(Buffer512 & buffer) const
	{
		SerializeData(buffer, m_gameId);
	}
	
	uint16_t PacketJoinGameRequest::EstimateCurrentSerializedSize() const
	{
		return sizeof(m_gameId);
	}
}
