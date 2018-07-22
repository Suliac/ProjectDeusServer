#include "PacketGetGamesRequest.h"

namespace DeusCore
{
	PacketGetGamesRequest::PacketGetGamesRequest()
		:Packet(Packet::EMessageType::GetGameRequest)
	{
	}

	PacketGetGamesRequest::~PacketGetGamesRequest()
	{
	}

	void PacketGetGamesRequest::OnDeserialize(Buffer512 & buffer)
	{
		// nothing to do, already done in base class
	}

	void PacketGetGamesRequest::OnSerialize(Buffer512 & buffer) const
	{
		// nothing to do, already done in base class
	}

	uint16_t PacketGetGamesRequest::EstimateCurrentSerializedSize() const
	{
		return 0x0;
	}
}
