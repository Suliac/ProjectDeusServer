#include "PacketLeaveGameAnswer.h"

namespace DeusCore
{
	PacketLeaveGameAnswer::PacketLeaveGameAnswer()
		:PacketAnswer(Packet::EMessageType::LeaveGameAnswer)
	{
	}
	
	PacketLeaveGameAnswer::~PacketLeaveGameAnswer()
	{
	}

	void PacketLeaveGameAnswer::OnAnswerDeserialize(Buffer512 & buffer)
	{
		DeserializeData(buffer, m_playerId);
	}

	void PacketLeaveGameAnswer::OnAnswerSerialize(Buffer512 & buffer) const
	{
		SerializeData(buffer, m_playerId);
	}

	uint16_t PacketLeaveGameAnswer::EstimateAnswerCurrentSerializedSize() const
	{
		return sizeof(m_playerId);
	}
}
