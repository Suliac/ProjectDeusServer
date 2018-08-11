#include "PacketUpdateMovementAnswer.h"

namespace DeusCore
{
	PacketUpdateMovementAnswer::PacketUpdateMovementAnswer(uint32_t objectId, uint32_t componentId, int32_t newHealthAmount)
		: DeusCore::PacketAnswer(Packet::EMessageType::UpdateHealth)
	{
		m_objectId = objectId;
		m_componentId = componentId;
	}
	
	PacketUpdateMovementAnswer::~PacketUpdateMovementAnswer()
	{
	}

	void PacketUpdateMovementAnswer::OnAnswerDeserialize(DeusCore::Buffer512 & buffer)
	{
		DeserializeData(buffer, m_objectId);
		DeserializeData(buffer, m_componentId);
	}

	void PacketUpdateMovementAnswer::OnAnswerSerialize(DeusCore::Buffer512 & buffer) const
	{
		SerializeData(buffer, m_objectId);
		SerializeData(buffer, m_componentId);
	}

	uint16_t PacketUpdateMovementAnswer::EstimateAnswerCurrentSerializedSize() const
	{
		return uint16_t(sizeof(m_objectId) + sizeof(m_componentId));
	}
}
