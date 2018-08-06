#include "PacketStartGame.h"

namespace DeusCore
{
	PacketStartGame::PacketStartGame()
		:Packet(EMessageType::PlayerReady)
	{
	}


	PacketStartGame::~PacketStartGame()
	{
	}

	void PacketStartGame::OnDeserialize(Buffer512 & buffer)
	{
		throw DeusCore::DeusSerializationException("This message isn't mean to be deserialized");
	}

	void PacketStartGame::OnSerialize(Buffer512 & buffer) const
	{
		throw DeusCore::DeusSerializationException("This message isn't mean to be deserialized");
	}

	uint16_t PacketStartGame::EstimateCurrentSerializedSize() const
	{
		return 0; //nothing to do
	}
}
