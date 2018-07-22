#pragma once
#include "PacketAnswer.h"
namespace DeusCore
{
	class PacketJoinGameAnswer : public PacketAnswer
	{
	public:
		PacketJoinGameAnswer();
		~PacketJoinGameAnswer();

		void SetGame(unsigned int value) { m_gameIdJoined = value; };
		unsigned int GetGameId() { return m_gameIdJoined; }
	protected:
		virtual void OnAnswerDeserialize(Buffer512 & buffer) override;
		virtual void OnAnswerSerialize(Buffer512 & buffer) const override;
		virtual uint16_t EstimateAnswerCurrentSerializedSize() const override;

	private:
		unsigned int m_gameIdJoined;
	};
}

