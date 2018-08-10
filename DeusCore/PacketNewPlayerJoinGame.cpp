#include "PacketNewPlayerJoinGame.h"


namespace DeusCore
{

	PacketNewPlayerJoinGame::PacketNewPlayerJoinGame()
		: Packet(Packet::EMessageType::NewPlayerJoin)
	{
	}

	PacketNewPlayerJoinGame::~PacketNewPlayerJoinGame()
	{
	}

	void PacketNewPlayerJoinGame::OnDeserialize(Buffer512 & buffer)
	{
		DeserializeData(buffer, m_playerId);

		// get the size of the string
		uint8_t dataSize;
		DeserializeData(buffer, dataSize);

		// get the string
		m_playerNickname.clear();
		std::vector<unsigned char> tmp = buffer.Select(dataSize);

		m_playerNickname.assign((char *)tmp.data());

	}

	void PacketNewPlayerJoinGame::OnSerialize(Buffer512 & buffer) const
	{
		SerializeData(buffer, m_playerId);

		uint8_t dataSize = m_playerNickname.size() + 1; // +1 to add the \0 of string
		SerializeData(buffer, dataSize); // SerializeData only for primitive

										 //  then we add the string 
		const char* myCurrentText = m_playerNickname.c_str();
		for (size_t i = 0; i < dataSize; i++)
		{
			SerializeData(buffer, *myCurrentText);
			myCurrentText++;
		}

	}

	uint16_t PacketNewPlayerJoinGame::EstimateCurrentSerializedSize() const
	{
		// 1 PacketClientConnected uses :
		// - 4 bytes					: to save player id
		// - 1 byte						: to save an unsigned int for the length of the next string
		// - m_message.size()+1 bytes	: to save the string ip address ('+1' is for the \0)
		return uint16_t(sizeof(Id) + sizeof(uint8_t) + (m_playerNickname.size() + 1));
	}
}
