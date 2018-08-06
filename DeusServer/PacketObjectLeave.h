#pragma once
#include "DeusCore/Packet.h"
namespace DeusServer
{
	class PacketObjectLeave : public DeusCore::Packet
	{
	public:
		PacketObjectLeave(uint32_t objectId);
		~PacketObjectLeave();

	protected:
		virtual void OnDeserialize(DeusCore::Buffer512 & buffer) override;
		virtual void OnSerialize(DeusCore::Buffer512 & buffer) const override;
		virtual uint16_t EstimateCurrentSerializedSize() const override;

	private:
		uint32_t m_objectId;
	};
}
