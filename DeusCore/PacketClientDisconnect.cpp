#include "PacketClientDisconnect.h"


namespace DeusCore
{
	PacketClientDisconnect::PacketClientDisconnect()
		:Packet(Packet::EMessageType::Disconnect)
	{
	}


	PacketClientDisconnect::~PacketClientDisconnect()
	{
	}

	void PacketClientDisconnect::OnDeserialize(Buffer512 & buffer)
	{
		throw DeusCore::DeusSerializationException("This message isn't mean to be deserialized");
	}

	void PacketClientDisconnect::OnSerialize(Buffer512 & buffer) const
	{
		throw DeusCore::DeusSerializationException("This message isn't mean to be deserialized");
	}

	uint16_t PacketClientDisconnect::EstimateCurrentSerializedSize() const
	{
		return 0x0;
	}
}
