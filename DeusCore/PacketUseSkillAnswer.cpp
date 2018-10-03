#include "PacketUseSkillAnswer.h"

namespace DeusCore
{
	PacketUseSkillAnswer::PacketUseSkillAnswer()
		: PacketAnswer(Packet::EMessageType::UseSkillAnswer)
	{
	}

	PacketUseSkillAnswer::~PacketUseSkillAnswer()
	{
	}

	void PacketUseSkillAnswer::OnAnswerDeserialize(Buffer512 & buffer)
	{
		DeserializeData(buffer, m_objectId);
		DeserializeData(buffer, m_skillId);
	}

	void PacketUseSkillAnswer::OnAnswerSerialize(Buffer512 & buffer) const
	{
		SerializeData(buffer, m_objectId);
		SerializeData(buffer, m_skillId);
	}

	uint16_t PacketUseSkillAnswer::EstimateAnswerCurrentSerializedSize() const
	{
		return uint16_t(sizeof(m_objectId) + sizeof(m_skillId));
	}
}
