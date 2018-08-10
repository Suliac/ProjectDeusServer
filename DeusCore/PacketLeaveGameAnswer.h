#pragma once
#include "Packets.h"

namespace DeusCore
{
	class PacketLeaveGameAnswer : public PacketAnswer
	{
	public:
		PacketLeaveGameAnswer();
		~PacketLeaveGameAnswer();

		void SetPlayerId(Id value) { m_playerId = value; }
		Id GetPlayerId() const { return m_playerId; }

	protected:
		virtual void OnAnswerDeserialize(Buffer512 & buffer) override;
		virtual void OnAnswerSerialize(Buffer512 & buffer) const override;
		virtual uint16_t EstimateAnswerCurrentSerializedSize() const override;
	
	private:
		Id m_playerId;
	};
}

