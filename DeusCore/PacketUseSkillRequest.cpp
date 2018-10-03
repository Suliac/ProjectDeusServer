#include "PacketUseSkillRequest.h"


namespace DeusCore
{

	PacketUseSkillRequest::PacketUseSkillRequest()
		: Packet(Packet::EMessageType::UseSkillRequest)
	{
	}

	PacketUseSkillRequest::~PacketUseSkillRequest()
	{
	}

	void PacketUseSkillRequest::OnDeserialize(Buffer512 & buffer)
	{
		DeserializeData(buffer, m_skillId);
	}

	void PacketUseSkillRequest::OnSerialize(Buffer512 & buffer) const
	{
		SerializeData(buffer, m_skillId);
	}

	uint16_t PacketUseSkillRequest::EstimateCurrentSerializedSize() const
	{
		return uint16_t(sizeof(m_skillId));
	}
}
