#pragma once
#include "PacketAnswer.h"

namespace DeusCore
{
	class PacketUpdateMovementAnswer: public DeusCore::PacketAnswer
	{
	public:
		PacketUpdateMovementAnswer(uint32_t objectId, uint32_t componentId, int32_t newHealthAmount);
		~PacketUpdateMovementAnswer();

	protected:
		virtual void OnAnswerDeserialize(DeusCore::Buffer512 & buffer) override;
		virtual void OnAnswerSerialize(DeusCore::Buffer512 & buffer) const override;
		virtual uint16_t EstimateAnswerCurrentSerializedSize() const override;

	private:
		uint32_t m_objectId;
		uint32_t m_componentId;
		
	};
}

