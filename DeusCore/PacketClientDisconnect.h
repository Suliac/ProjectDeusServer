#pragma once
#include "Packet.h"

namespace DeusCore
{
	class PacketClientDisconnect : public Packet
	{
	public:
		PacketClientDisconnect();
		~PacketClientDisconnect();
		
	protected:
		virtual void OnDeserialize(Buffer512 & buffer) override;
		virtual void OnSerialize(Buffer512 & buffer) const override;
		virtual uint16_t EstimateCurrentSerializedSize() const override;

	};
}

