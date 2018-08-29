#include "PacketGameStarted.h"


namespace DeusCore
{
	PacketGameStarted::PacketGameStarted()
		: Packet(Packet::EMessageType::GameStarted)
	{
	}
	
	PacketGameStarted::~PacketGameStarted()
	{
	}

	void PacketGameStarted::OnDeserialize(Buffer512 & buffer)
	{
		// nothing to do
	}

	void PacketGameStarted::OnSerialize(Buffer512 & buffer) const
	{
		// nothing to do
	}

	uint16_t PacketGameStarted::EstimateCurrentSerializedSize() const
	{
		return uint16_t();
	}
}
