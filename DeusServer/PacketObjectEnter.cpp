#include "PacketObjectEnter.h"

namespace DeusServer
{
	PacketObjectEnter::PacketObjectEnter(uint32_t objectId, GameObject::EObjectType objectType)
		: DeusCore::Packet(Packet::EMessageType::ObjectEnter)
	{
		m_objectId = objectId;
		m_objectType = objectType;
	}

	PacketObjectEnter::~PacketObjectEnter()
	{
	}

	void PacketObjectEnter::OnDeserialize(DeusCore::Buffer512 & buffer)
	{
		DeserializeData(buffer, m_objectId);

		uint8_t objectType = m_objectType;
		DeserializeData(buffer, objectType);
		m_objectType = (GameObject::EObjectType)objectType;
	}

	void PacketObjectEnter::OnSerialize(DeusCore::Buffer512 & buffer) const
	{
		SerializeData(buffer, m_objectId);

		uint8_t objectType = m_objectType;
		SerializeData(buffer, objectType);
	}

	uint16_t PacketObjectEnter::EstimateCurrentSerializedSize() const
	{
		return uint16_t(sizeof(m_objectId) + sizeof(m_objectType));
	}
}
