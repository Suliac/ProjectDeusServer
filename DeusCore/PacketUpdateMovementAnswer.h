#pragma once
#include "PacketAnswer.h"
#include "DeusVector2.h"
namespace DeusCore
{
	class PacketUpdateMovementAnswer: public DeusCore::PacketAnswer
	{
	public:
		PacketUpdateMovementAnswer(Id objectId, Id componentId,
			DeusVector2 origin, uint32_t originMs, DeusVector2 dest, uint32_t destMs);
		
		PacketUpdateMovementAnswer();
		~PacketUpdateMovementAnswer();

	protected:
		virtual void OnAnswerDeserialize(DeusCore::Buffer512 & buffer) override;
		virtual void OnAnswerSerialize(DeusCore::Buffer512 & buffer) const override;
		virtual uint16_t EstimateAnswerCurrentSerializedSize() const override;

	private:
		Id m_objectId;
		Id m_componentId;

		DeusVector2 m_originPos;
		uint32_t m_originPosMs;

		DeusVector2 m_destPos;
		uint32_t m_destPosMs;
	};
}

