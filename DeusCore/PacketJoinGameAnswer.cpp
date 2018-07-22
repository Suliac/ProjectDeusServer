#include "PacketJoinGameAnswer.h"


namespace DeusCore
{
	PacketJoinGameAnswer::PacketJoinGameAnswer()
		: PacketAnswer(Packet::EMessageType::JoinGameAnswer)
	{
	}


	PacketJoinGameAnswer::~PacketJoinGameAnswer()
	{
	}

	void PacketJoinGameAnswer::OnAnswerDeserialize(Buffer512 & buffer)
	{
		DeserializeData(buffer, m_gameIdJoined);
	}

	void PacketJoinGameAnswer::OnAnswerSerialize(Buffer512 & buffer) const
	{
		SerializeData(buffer, m_gameIdJoined);
	}

	uint16_t PacketJoinGameAnswer::EstimateAnswerCurrentSerializedSize() const
	{
		return sizeof(m_gameIdJoined);
	}
}
