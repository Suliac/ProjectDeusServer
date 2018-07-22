#pragma once
#include "Packet.h"

namespace DeusCore
{
	class PacketCreateGameRequest : public Packet
	{
	public:
		PacketCreateGameRequest();
		~PacketCreateGameRequest();

	protected:
		virtual void OnDeserialize(Buffer512 & buffer) override;
		virtual void OnSerialize(Buffer512 & buffer) const override;
		virtual uint16_t EstimateCurrentSerializedSize() const override;
		
	};
}

