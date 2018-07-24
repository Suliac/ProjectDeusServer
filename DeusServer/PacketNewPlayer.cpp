#include "PacketNewPlayer.h"

namespace DeusServer
{
	PacketNewPlayer::PacketNewPlayer(std::shared_ptr<DeusClient> clientConnection)
		: DeusCore::Packet(DeusCore::Packet::EMessageType::NewPlayer),
		m_clientConnection(clientConnection)
	{
	}
	
	PacketNewPlayer::~PacketNewPlayer()
	{
	}

	void PacketNewPlayer::OnDeserialize(DeusCore::Buffer512 & buffer)
	{
		throw DeusCore::DeusSerializationException("This message isn't mean to be deserialized");
	}
	void PacketNewPlayer::OnSerialize(DeusCore::Buffer512 & buffer) const
	{
		throw DeusCore::DeusSerializationException("This message isn't mean to be serialized");
	}
	uint16_t PacketNewPlayer::EstimateCurrentSerializedSize() const
	{
		return 0;
	}
}
