#include "PacketObjectLeave.h"


namespace DeusServer
{
	PacketObjectLeave::PacketObjectLeave(uint32_t objectId)
		: DeusCore::Packet(Packet::EMessageType::ObjectLeave)
	{
		m_objectId = objectId;
	}

	PacketObjectLeave::~PacketObjectLeave()
	{
	}

	void PacketObjectLeave::OnDeserialize(DeusCore::Buffer512 & buffer)
	{
		DeserializeData(buffer, m_objectId);
	}

	void PacketObjectLeave::OnSerialize(DeusCore::Buffer512 & buffer) const
	{
		SerializeData(buffer, m_objectId);
	}

	uint16_t PacketObjectLeave::EstimateCurrentSerializedSize() const
	{
		return uint16_t(sizeof(m_objectId));
	}
}
