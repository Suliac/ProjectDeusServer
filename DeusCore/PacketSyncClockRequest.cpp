#include "PacketSyncClockRequest.h"

namespace DeusCore
{
	PacketSyncClockRequest::PacketSyncClockRequest()
		: Packet(EMessageType::ClockSyncRequest)
	{
	}

	PacketSyncClockRequest::~PacketSyncClockRequest()
	{
	}

	void PacketSyncClockRequest::OnDeserialize(Buffer512 & buffer)
	{
	}

	void PacketSyncClockRequest::OnSerialize(Buffer512 & buffer) const
	{
	}

	uint16_t PacketSyncClockRequest::EstimateCurrentSerializedSize() const
	{
		return uint16_t();
	}
}
