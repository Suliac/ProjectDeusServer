#pragma once
#include "DeusCore/Packet.h"

namespace DeusServer
{
	class PacketUpdateHealth : public DeusCore::Packet
	{
	public:
		PacketUpdateHealth(uint32_t objectId, uint32_t componentId, int32_t newHealthAmount);
		~PacketUpdateHealth();

	protected:
		virtual void OnDeserialize(DeusCore::Buffer512 & buffer) override;
		virtual void OnSerialize(DeusCore::Buffer512 & buffer) const override;
		virtual uint16_t EstimateCurrentSerializedSize() const override;

	private:
		uint32_t m_objectId;
		uint32_t m_componentId;
		int32_t m_newHealthAmount;
	};
}

