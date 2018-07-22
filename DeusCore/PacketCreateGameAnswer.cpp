#include "PacketCreateGameAnswer.h"


namespace DeusCore
{
	PacketCreateGameAnswer::PacketCreateGameAnswer()
		: PacketAnswer(Packet::EMessageType::CreateGameAnswer)
	{
	}


	PacketCreateGameAnswer::~PacketCreateGameAnswer()
	{
	}

	void PacketCreateGameAnswer::OnAnswerDeserialize(Buffer512 & buffer)
	{
		// nothing to do, already done in base class
	}

	void PacketCreateGameAnswer::OnAnswerSerialize(Buffer512 & buffer) const
	{
		// nothing to do, already done in base class
	}

	uint16_t PacketCreateGameAnswer::EstimateAnswerCurrentSerializedSize() const
	{
		return 0x0;
	}
}
