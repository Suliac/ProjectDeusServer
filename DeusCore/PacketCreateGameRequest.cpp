#include "PacketCreateGameRequest.h"



namespace DeusCore
{
	PacketCreateGameRequest::PacketCreateGameRequest() 
		: Packet(DeusCore::Packet::EMessageType::CreateGameRequest)
	{
	}


	PacketCreateGameRequest::~PacketCreateGameRequest()
	{
	}

	void PacketCreateGameRequest::OnDeserialize(Buffer512 & buffer)
	{
		// nothing to do, already done in base class
	}

	void PacketCreateGameRequest::OnSerialize(Buffer512 & buffer) const
	{
		// nothing to do, already done in base class
	}

	uint16_t PacketCreateGameRequest::EstimateCurrentSerializedSize() const
	{
		return 0x0;
	}
}
