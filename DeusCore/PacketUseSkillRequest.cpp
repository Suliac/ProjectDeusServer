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
		DeserializeData(buffer, m_componentId);
		DeserializeData(buffer, m_skillId);
		DeserializeData<ISerializable>(buffer, m_skillLaunchPosition);
	}

	void PacketUseSkillRequest::OnSerialize(Buffer512 & buffer) const
	{
		SerializeData(buffer, m_componentId);
		SerializeData(buffer, m_skillId);
		SerializeData<ISerializable>(buffer, m_skillLaunchPosition);
	}

	uint16_t PacketUseSkillRequest::EstimateCurrentSerializedSize() const
	{
		return uint16_t(sizeof(m_skillId) + sizeof(m_skillLaunchPosition) + sizeof(m_componentId));
	}
}
