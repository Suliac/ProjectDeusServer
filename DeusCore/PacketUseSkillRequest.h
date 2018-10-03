#pragma once
#include "Packet.h"
namespace DeusCore
{
	class PacketUseSkillRequest : public Packet
	{
	public:
		PacketUseSkillRequest();
		~PacketUseSkillRequest();

	protected:
		virtual void OnDeserialize(Buffer512 & buffer) override;
		virtual void OnSerialize(Buffer512 & buffer) const override;
		virtual uint16_t EstimateCurrentSerializedSize() const override;

	private:
		Id m_skillId;
	};
}

