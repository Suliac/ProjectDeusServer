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
	}
	void PacketLeaveGameAnswer::OnAnswerSerialize(Buffer512 & buffer) const
	{
	}
	uint16_t PacketLeaveGameAnswer::EstimateAnswerCurrentSerializedSize() const
	{
		return 0x0;
	}
}
