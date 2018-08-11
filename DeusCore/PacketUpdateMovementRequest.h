#pragma once
#include "Packet.h"
#include "DeusVector2.h"

namespace DeusCore
{
	class PacketUpdateMovementRequest : public Packet
	{
	public:
		PacketUpdateMovementRequest();
		PacketUpdateMovementRequest(DeusVector2 newDir);

		~PacketUpdateMovementRequest();

		const DeusVector2& GetNewDir() const { return m_newDir; }

	protected:
		virtual void OnDeserialize(Buffer512 & buffer) override;
		virtual void OnSerialize(Buffer512 & buffer) const override;
		virtual uint16_t EstimateCurrentSerializedSize() const override;

	private:
		DeusVector2 m_newDir;
	};
}

