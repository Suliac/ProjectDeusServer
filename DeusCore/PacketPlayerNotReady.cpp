#include "PacketPlayerNotReady.h"


namespace DeusCore
{
	PacketPlayerNotReady::PacketPlayerNotReady()
		:Packet(EMessageType::PlayerNotReady)
	{
	}


	PacketPlayerNotReady::~PacketPlayerNotReady()
	{
	}

	void PacketPlayerNotReady::OnDeserialize(Buffer512 & buffer)
	{
		//nothing to do
	}

	void PacketPlayerNotReady::OnSerialize(Buffer512 & buffer) const
	{
		//nothing to do
	}

	uint16_t PacketPlayerNotReady::EstimateCurrentSerializedSize() const
	{
		return 0; //nothing to do
	}
}
