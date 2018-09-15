#include "PacketPingRequest.h"

namespace DeusCore
{
	PacketPingRequest::PacketPingRequest()
		: Packet(Packet::EMessageType::PingRequest)
	{
	}

	PacketPingRequest::~PacketPingRequest()
	{
	}

	void PacketPingRequest::OnDeserialize(Buffer512 & buffer)
	{
	}

	void PacketPingRequest::OnSerialize(Buffer512 & buffer) const
	{
	}

	uint16_t PacketPingRequest::EstimateCurrentSerializedSize() const
	{
		return uint16_t();
	}
}
