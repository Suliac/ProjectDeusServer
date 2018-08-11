#include "PacketUpdateMovementRequest.h"

namespace DeusCore
{
	PacketUpdateMovementRequest::PacketUpdateMovementRequest(DeusVector2 newDir)
		: Packet(Packet::EMessageType::UpdateHealth)
	{
		m_newDir = newDir;
	}

	PacketUpdateMovementRequest::PacketUpdateMovementRequest()
		: Packet(Packet::EMessageType::UpdateHealth)
	{
	}
	
	PacketUpdateMovementRequest::~PacketUpdateMovementRequest()
	{
	}

	void PacketUpdateMovementRequest::OnDeserialize(Buffer512 & buffer)
	{
		DeserializeData<ISerializable>(buffer, m_newDir);
	}

	void PacketUpdateMovementRequest::OnSerialize(Buffer512 & buffer) const
	{
		SerializeData<ISerializable>(buffer, m_newDir);
	}

	uint16_t PacketUpdateMovementRequest::EstimateCurrentSerializedSize() const
	{
		return m_newDir.EstimateAnswerCurrentSerializedSize();
	}
}
