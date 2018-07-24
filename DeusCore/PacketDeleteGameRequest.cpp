#include "PacketDeleteGameRequest.h"


namespace DeusCore
{

	PacketDeleteGameRequest::PacketDeleteGameRequest()
		: Packet(Packet::EMessageType::DeleteGameRequest)
	{
	}


	PacketDeleteGameRequest::~PacketDeleteGameRequest()
	{
	}

	void PacketDeleteGameRequest::OnDeserialize(Buffer512 & buffer)
	{
	}

	void PacketDeleteGameRequest::OnSerialize(Buffer512 & buffer) const
	{
	}

	uint16_t PacketDeleteGameRequest::EstimateCurrentSerializedSize() const
	{
		return 0x0;
	}
}
