#pragma once
#include "PacketAnswer.h"
namespace DeusCore
{
	class PacketUseSkillAnswer : public PacketAnswer
	{
	public:
		PacketUseSkillAnswer();
		~PacketUseSkillAnswer();

	protected:
		virtual void OnAnswerDeserialize(Buffer512 & buffer) override;
		virtual void OnAnswerSerialize(Buffer512 & buffer) const override;
		virtual uint16_t EstimateAnswerCurrentSerializedSize() const override;

	private:
		Id m_objectId;
		Id m_skillId;
	};
}

