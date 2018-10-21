#pragma once
#include "Packet.h"
#include "DeusVector2.h"
namespace DeusCore
{
	class PacketUseSkillRequest : public Packet
	{
	public:
		PacketUseSkillRequest();
		~PacketUseSkillRequest();

		Id GetComponentId() const { return m_componentId; }
		Id GetSkillId() const { return m_skillId; }
		DeusVector2 GetSkillPosition() const { return m_skillLaunchPosition; }

	protected:
		virtual void OnDeserialize(Buffer512 & buffer) override;
		virtual void OnSerialize(Buffer512 & buffer) const override;
		virtual uint16_t EstimateCurrentSerializedSize() const override;

	private:
		Id m_componentId;
		Id m_skillId;
		DeusVector2 m_skillLaunchPosition;
	};
}

