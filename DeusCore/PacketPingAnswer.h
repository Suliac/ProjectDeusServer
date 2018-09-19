#pragma once
#include "Packet.h"
namespace DeusCore
{
	class PacketPingAnswer : public Packet
	{
	public:
		PacketPingAnswer();
		~PacketPingAnswer();

		void SetPacketRecvId(uint32_t value) { m_responseToPacketId = value; }

	protected:
		virtual void OnDeserialize(Buffer512 & buffer) override;
		virtual void OnSerialize(Buffer512 & buffer) const override;
		virtual uint16_t EstimateCurrentSerializedSize() const override;

	private:
		uint32_t m_responseToPacketId;
	};
}

