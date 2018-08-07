#include "PacketObjectEnter.h"

namespace DeusServer
{
	PacketObjectEnter::PacketObjectEnter(Id objectId, GameObject::EObjectType objectType, bool isLocalPlayer)
		: DeusCore::Packet(Packet::EMessageType::ObjectEnter)
	{
		m_objectId = objectId;
		m_objectType = objectType;
		m_isLocalPlayer = isLocalPlayer;
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

		DeserializeData(buffer, m_isLocalPlayer);
	}

	void PacketObjectEnter::OnSerialize(DeusCore::Buffer512 & buffer) const
	{
		SerializeData(buffer, m_objectId);

		uint8_t objectType = m_objectType;
		SerializeData(buffer, objectType);

		SerializeData(buffer, m_isLocalPlayer);
	}

	uint16_t PacketObjectEnter::EstimateCurrentSerializedSize() const
	{
		return uint16_t(sizeof(m_objectId) + sizeof(m_objectType) + sizeof(m_isLocalPlayer));
	}
}
