#include "PacketJoinGameAnswer.h"


namespace DeusCore
{
	PacketJoinGameAnswer::PacketJoinGameAnswer()
		: PacketAnswer(Packet::EMessageType::JoinGameAnswer)
	{
	}


	PacketJoinGameAnswer::~PacketJoinGameAnswer()
	{
	}

	void PacketJoinGameAnswer::OnAnswerDeserialize(Buffer512 & buffer)
	{
		throw new DeusSerializationException("Don't try to deserialize this packet on server side");
	}

	void PacketJoinGameAnswer::OnAnswerSerialize(Buffer512 & buffer) const
	{
		SerializeData(buffer, m_gameIdJoined);

		uint16_t sizePlayers = m_playerInfos.size();
		SerializeData(buffer, sizePlayers);

		for (const auto& playerInfo : m_playerInfos)
		{
			SerializeData(buffer, playerInfo.first);

			uint8_t dataSize = playerInfo.second.size() + 1; // +1 to add the \0 of string
			SerializeData(buffer, dataSize); // SerializeData only for primitive

			//  then we add the string 
			const char* myCurrentText = playerInfo.second.c_str();
			for (size_t i = 0; i < dataSize; i++)
			{
				SerializeData(buffer, *myCurrentText);
				myCurrentText++;
			}
		}
	}

	uint16_t PacketJoinGameAnswer::EstimateAnswerCurrentSerializedSize() const
	{
		uint16_t sizeStr = 0;
		for (const auto& playerInfo : m_playerInfos)
		{
			sizeStr += sizeof(uint8_t); // size of the str is saved
			sizeStr += playerInfo.second.size() + 1;
		}

		// 4 bytes	for gameId joined
		// 2 bytes	for number of playerinfos
		// 4 bytes	for each player id
		// X bytes	for each player nickname
		return uint16_t(sizeof(m_gameIdJoined) + sizeof(uint16_t) + (m_playerInfos.size() * sizeof(uint32_t)) + sizeStr);
	}
}
