#pragma once
#include "Packets.h"
namespace DeusCore
{
	class PacketLeaveGameRequest : public Packet
	{
	public:
		PacketLeaveGameRequest();
		~PacketLeaveGameRequest();

	protected:
		virtual void OnDeserialize(Buffer512 & buffer) override;
		virtual void OnSerialize(Buffer512 & buffer) const override;
		virtual uint16_t EstimateCurrentSerializedSize() const override;
	};
}

