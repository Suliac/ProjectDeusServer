#pragma once
#include "PacketAnswer.h"
#include "DeusVector2.h"
namespace DeusCore
{
	class PacketUseSkillAnswer : public PacketAnswer
	{
	public:
		PacketUseSkillAnswer();
		PacketUseSkillAnswer(Id objectId, Id skillId, const DeusVector2& pos);
		~PacketUseSkillAnswer();

		void SetSkillId(Id value) { m_skillId = value; }
		void SetObjectId(Id value) { m_objectId = value; }
		void SetSkillPosition(const DeusVector2& value) { m_skillLaunchPosition = value; }

	protected:
		virtual void OnAnswerDeserialize(Buffer512 & buffer) override;
		virtual void OnAnswerSerialize(Buffer512 & buffer) const override;
		virtual uint16_t EstimateAnswerCurrentSerializedSize() const override;

	private:
		Id m_objectId;
		Id m_skillId;
		DeusVector2 m_skillLaunchPosition;
	};
}

