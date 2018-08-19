#pragma once
#include "PacketAnswer.h"
#include "DeusVector2.h"
namespace DeusCore
{
	class PacketUpdateMovementAnswer: public DeusCore::PacketAnswer
	{
	public:
		PacketUpdateMovementAnswer(Id objectId, Id componentId,
			DeusVector2 origin, unsigned long originMs, DeusVector2 dest, unsigned long destMs);
		
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
		unsigned long m_originPosMs;

		DeusVector2 m_destPos;
		unsigned long m_destPosMs;
	};
}

