#pragma once
#include "Packet.h"
namespace DeusCore
{
	class PacketClientConnected : public Packet
	{
	public:
		PacketClientConnected();
		~PacketClientConnected();

		const std::string& GetAddrUdp() const { return m_addrUdp; }
		void SetAddrUdp(const std::string& value) { m_addrUdp = value; }

		uint32_t GetPortUdp() const { return m_portUdp; }
		void SetPortUdp(uint32_t value) { m_portUdp = value; }

	protected:
		virtual void OnDeserialize(Buffer512 & buffer) override;
		virtual void OnSerialize(Buffer512 & buffer) const override;
		virtual uint16_t EstimateCurrentSerializedSize() const override;

	private:
		std::string m_addrUdp;
		uint32_t m_portUdp;
	};
}

