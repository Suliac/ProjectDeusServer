#include "PacketUpdateMovementRequest.h"

namespace DeusCore
{
	PacketUpdateMovementRequest::PacketUpdateMovementRequest()
		: Packet(Packet::EMessageType::UpdateMovementRequest)
	{
	}

	PacketUpdateMovementRequest::~PacketUpdateMovementRequest()
	{
	}

	void PacketUpdateMovementRequest::OnDeserialize(Buffer512 & buffer)
	{
		DeserializeData(buffer, m_componentId);
		DeserializeData<ISerializable>(buffer, m_newPos);
	}

	void PacketUpdateMovementRequest::OnSerialize(Buffer512 & buffer) const
	{
		SerializeData(buffer, m_componentId);
		SerializeData<ISerializable>(buffer, m_newPos);
	}

	uint16_t PacketUpdateMovementRequest::EstimateCurrentSerializedSize() const
	{
		return sizeof(m_componentId) + m_newPos.EstimateAnswerCurrentSerializedSize();
	}
}
