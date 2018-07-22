#include "PacketAnswer.h"

namespace DeusCore
{

	PacketAnswer::PacketAnswer(Packet::EMessageType type)
		: Packet(type)
	{
	}
	
	PacketAnswer::~PacketAnswer()
	{
	}

	void PacketAnswer::OnDeserialize(Buffer512 & buffer)
	{
		DeserializeData(buffer, m_isSuccess);

		OnAnswerDeserialize(buffer);
	}

	void PacketAnswer::OnSerialize(Buffer512 & buffer) const
	{
		SerializeData(buffer, m_isSuccess);

		OnAnswerSerialize(buffer);
	}

	uint16_t PacketAnswer::EstimateCurrentSerializedSize() const
	{
		return sizeof(m_isSuccess) + EstimateAnswerCurrentSerializedSize();
	}

}
