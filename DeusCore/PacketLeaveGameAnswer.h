#pragma once
#include "Packets.h"

namespace DeusCore
{
	class PacketLeaveGameAnswer : public PacketAnswer
	{
	public:
		PacketLeaveGameAnswer();
		~PacketLeaveGameAnswer();

	protected:
		virtual void OnAnswerDeserialize(Buffer512 & buffer) override;
		virtual void OnAnswerSerialize(Buffer512 & buffer) const override;
		virtual uint16_t EstimateAnswerCurrentSerializedSize() const override;
	};
}

