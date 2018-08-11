#include "PacketUpdateHealth.h"

namespace DeusCore
{
	PacketUpdateHealth::PacketUpdateHealth(uint32_t objectId, uint32_t componentId, int32_t newHealthAmount)
		: DeusCore::Packet(Packet::EMessageType::UpdateHealth)
	{
		m_objectId = objectId;
		m_componentId = componentId;
		m_newHealthAmount = newHealthAmount;
	}
	
	PacketUpdateHealth::~PacketUpdateHealth()
	{
	}

	void PacketUpdateHealth::OnDeserialize(DeusCore::Buffer512 & buffer)
	{
		DeserializeData(buffer, m_objectId);
		DeserializeData(buffer, m_componentId);
		DeserializeData(buffer, m_newHealthAmount);
	}

	void PacketUpdateHealth::OnSerialize(DeusCore::Buffer512 & buffer) const
	{
		SerializeData(buffer, m_objectId);
		SerializeData(buffer, m_componentId);
		SerializeData(buffer, m_newHealthAmount);
	}

	uint16_t PacketUpdateHealth::EstimateCurrentSerializedSize() const
	{
		return uint16_t(sizeof(m_objectId) + sizeof(m_componentId) + sizeof(m_newHealthAmount));
	}
}
