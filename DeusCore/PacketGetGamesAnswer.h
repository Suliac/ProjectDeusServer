#pragma once
#include "PacketAnswer.h"
namespace DeusCore
{
	class PacketGetGamesAnswer : public PacketAnswer
	{
	public:
		PacketGetGamesAnswer();
		~PacketGetGamesAnswer();

		void SetGames(const std::vector<unsigned int>& gamesIds) { m_gamesIds = gamesIds; };
		const std::vector<unsigned int>& GetGames() { return m_gamesIds; }
	protected:
		virtual void OnAnswerDeserialize(Buffer512 & buffer) override;
		virtual void OnAnswerSerialize(Buffer512 & buffer) const override;
		virtual uint16_t EstimateAnswerCurrentSerializedSize() const override;

	private:
		std::vector<unsigned int> m_gamesIds;
	};
}

