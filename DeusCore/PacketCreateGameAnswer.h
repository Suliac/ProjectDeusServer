#pragma once
#include "PacketAnswer.h"
namespace DeusCore
{
	class PacketCreateGameAnswer : public PacketAnswer
	{
	public:
		PacketCreateGameAnswer();
		~PacketCreateGameAnswer();

	protected:
		virtual void OnAnswerDeserialize(Buffer512 & buffer) override;
		virtual void OnAnswerSerialize(Buffer512 & buffer) const override;
		virtual uint16_t EstimateAnswerCurrentSerializedSize() const override;
	};
}

