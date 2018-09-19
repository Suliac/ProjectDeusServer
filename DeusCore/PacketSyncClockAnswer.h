#pragma once
#include "Packet.h"
namespace DeusCore
{
	class PacketSyncClockAnswer : public Packet
	{
	public:
		PacketSyncClockAnswer();
		~PacketSyncClockAnswer();

		void SetServerTimeStamp(uint32_t value) { m_currentServerTimeStamp = value; }

	protected:
		virtual void OnDeserialize(Buffer512 & buffer) override;
		virtual void OnSerialize(Buffer512 & buffer) const override;
		virtual uint16_t EstimateCurrentSerializedSize() const override;

	private:
		uint32_t m_currentServerTimeStamp;
	};
}

