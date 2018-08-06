#include "PacketPlayerReady.h"


namespace DeusCore
{
	PacketPlayerReady::PacketPlayerReady()
		:Packet(EMessageType::PlayerReady)
	{
	}


	PacketPlayerReady::~PacketPlayerReady()
	{
	}

	void PacketPlayerReady::OnDeserialize(Buffer512 & buffer)
	{
		//nothing to do
	}

	void PacketPlayerReady::OnSerialize(Buffer512 & buffer) const
	{
		//nothing to do
	}

	uint16_t PacketPlayerReady::EstimateCurrentSerializedSize() const
	{
		return 0; //nothing to do
	}
}
