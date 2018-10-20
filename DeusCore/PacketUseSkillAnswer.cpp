#include "PacketUseSkillAnswer.h"

namespace DeusCore
{
	PacketUseSkillAnswer::PacketUseSkillAnswer()
		: PacketAnswer(Packet::EMessageType::UseSkillAnswer)
	{
	}

	PacketUseSkillAnswer::PacketUseSkillAnswer(Id objectId, Id skillId, const DeusVector2& pos, uint32_t timeLaunched)
		: PacketAnswer(Packet::EMessageType::UseSkillAnswer)		
	{
		m_objectId = objectId;
		m_skillId = skillId;
		m_skillLaunchPosition = pos;
		m_skillLaunchMs = timeLaunched;
	}

	PacketUseSkillAnswer::~PacketUseSkillAnswer()
	{
	}

	void PacketUseSkillAnswer::OnAnswerDeserialize(Buffer512 & buffer)
	{
		DeserializeData(buffer, m_objectId);
		DeserializeData(buffer, m_skillId);
		DeserializeData<ISerializable>(buffer, m_skillLaunchPosition);
		DeserializeData(buffer, m_skillId);

	}

	void PacketUseSkillAnswer::OnAnswerSerialize(Buffer512 & buffer) const
	{
		SerializeData(buffer, m_objectId);
		SerializeData(buffer, m_skillId);
		SerializeData<ISerializable>(buffer, m_skillLaunchPosition);
		SerializeData(buffer, m_skillId);

	}

	uint16_t PacketUseSkillAnswer::EstimateAnswerCurrentSerializedSize() const
	{
		return uint16_t(sizeof(m_objectId) + sizeof(m_skillId) + sizeof(m_skillLaunchPosition)) + sizeof(m_skillLaunchMs);
	}
}
