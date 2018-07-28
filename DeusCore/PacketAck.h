#pragma once 
#include "Packet.h"
namespace DeusCore
{
	class PacketAck : public Packet
	{
	public:
		PacketAck();
		~PacketAck();

		void SetPacketToAck(uint32_t value) { m_packetToAck = value; }
		uint32_t GetPacketToAck() const { return m_packetToAck; }

	protected:
		virtual void OnDeserialize(Buffer512 & buffer) override;
		virtual void OnSerialize(Buffer512 & buffer) const override;
		virtual uint16_t EstimateCurrentSerializedSize() const override;

	private:
		uint32_t m_packetToAck;
	};
}

