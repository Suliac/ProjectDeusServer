#pragma once
#include "Packet.h"
#include "DeusVector2.h"

namespace DeusCore
{
	class PacketUpdateMovementRequest : public Packet
	{
	public:
		PacketUpdateMovementRequest();
		~PacketUpdateMovementRequest();

		const DeusVector2& GetNewPosition() const { return m_newPos; }
		Id GetComponentId() const { return m_componentId; }

	protected:
		virtual void OnDeserialize(Buffer512 & buffer) override;
		virtual void OnSerialize(Buffer512 & buffer) const override;
		virtual uint16_t EstimateCurrentSerializedSize() const override;

	private:
		DeusVector2 m_newPos;
		Id m_componentId;
	};
}

