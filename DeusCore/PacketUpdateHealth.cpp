#include "PacketUpdateHealth.h"

namespace DeusCore
{
	PacketUpdateHealth::PacketUpdateHealth(uint32_t objectId, uint32_t componentId, int32_t newHealthAmount, uint32_t timestamp)
		: DeusCore::Packet(Packet::EMessageType::UpdateHealth)
	{
		m_objectId = objectId;
		m_componentId = componentId;
		m_newHealthAmount = newHealthAmount;
		m_newHealthTimestamp = timestamp;
	}
	
	PacketUpdateHealth::~PacketUpdateHealth()
	{
	}

	void PacketUpdateHealth::OnDeserialize(DeusCore::Buffer512 & buffer)
	{
		DeserializeData(buffer, m_objectId);
		DeserializeData(buffer, m_componentId);
		DeserializeData(buffer, m_newHealthAmount);
		DeserializeData(buffer, m_newHealthTimestamp);
	}

	void PacketUpdateHealth::OnSerialize(DeusCore::Buffer512 & buffer) const
	{
		SerializeData(buffer, m_objectId);
		SerializeData(buffer, m_componentId);
		SerializeData(buffer, m_newHealthAmount);
		SerializeData(buffer, m_newHealthTimestamp);
	}

	uint16_t PacketUpdateHealth::EstimateCurrentSerializedSize() const
	{
		return uint16_t(sizeof(m_objectId) + sizeof(m_componentId) + sizeof(m_newHealthAmount) + sizeof(m_newHealthTimestamp));
	}
}
