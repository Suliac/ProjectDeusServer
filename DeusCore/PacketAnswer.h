#pragma once
#include "Packet.h"
namespace DeusCore
{
	class PacketAnswer : public Packet
	{
	public:
		PacketAnswer(Packet::EMessageType type);
		~PacketAnswer();

		bool GetSuccess() { return m_isSuccess; }
		void SetSuccess(bool value) { m_isSuccess = value; }
	protected:
		virtual void OnDeserialize(Buffer512 & buffer) override final;
		virtual void OnSerialize(Buffer512 & buffer) const override final;
		virtual uint16_t EstimateCurrentSerializedSize() const override final;

		virtual void OnAnswerDeserialize(Buffer512 & buffer) = 0;
		virtual void OnAnswerSerialize(Buffer512 & buffer) const = 0;
		virtual uint16_t EstimateAnswerCurrentSerializedSize() const = 0;

		bool m_isSuccess;

	};
}

