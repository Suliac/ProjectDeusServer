#include "PacketGetGamesAnswer.h"


namespace DeusCore
{
	PacketGetGamesAnswer::PacketGetGamesAnswer()
		: PacketAnswer(Packet::EMessageType::GetGameAnswer)
	{
	}


	PacketGetGamesAnswer::~PacketGetGamesAnswer()
	{
	}
	
	void PacketGetGamesAnswer::OnAnswerDeserialize(Buffer512 & buffer)
	{
		size_t gamesNumber;
		DeserializeData(buffer, gamesNumber);

		for (size_t i = 0; i < gamesNumber; i++)
		{
			unsigned int gameId;
			DeserializeData(buffer, gameId);

			m_gamesIds.push_back(gameId);
		}
	}

	void PacketGetGamesAnswer::OnAnswerSerialize(Buffer512 & buffer) const
	{
		SerializeData(buffer, m_gamesIds.size());

		for (size_t i = 0; i < m_gamesIds.size(); i++)
		{
			SerializeData(buffer, m_gamesIds[i]);
		}
	}

	uint16_t PacketGetGamesAnswer::EstimateAnswerCurrentSerializedSize() const
	{
		return sizeof(size_t) + (m_gamesIds.size() * sizeof(unsigned int));
	}
}
