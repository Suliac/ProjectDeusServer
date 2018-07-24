#include "PacketLeaveGameRequest.h"

namespace DeusCore
{
	PacketLeaveGameRequest::PacketLeaveGameRequest()
		: Packet(Packet::EMessageType::LeaveGameRequest)
	{
	}

	PacketLeaveGameRequest::~PacketLeaveGameRequest()
	{
	}

	void PacketLeaveGameRequest::OnDeserialize(Buffer512 & buffer)
	{
	}

	void PacketLeaveGameRequest::OnSerialize(Buffer512 & buffer) const
	{
	}

	uint16_t PacketLeaveGameRequest::EstimateCurrentSerializedSize() const
	{
		return 0x0;
	}
}
