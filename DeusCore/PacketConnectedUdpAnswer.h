#pragma once
#include "Packet.h"
namespace DeusCore
{
	class PacketConnectedUdpAnswer : public Packet
	{
	public:
		PacketConnectedUdpAnswer(std::string nickname)
			: Packet(Packet::EMessageType::ConnectedUdpAnswer)
		{
			m_playerNickname = nickname;
		}

		PacketConnectedUdpAnswer();
		~PacketConnectedUdpAnswer();

		const std::string& GetNickname() const { return m_playerNickname; }
		void SetNickname(const std::string& value) { m_playerNickname = value; }
		
	protected:
		virtual void OnDeserialize(Buffer512 & buffer) override;
		virtual void OnSerialize(Buffer512 & buffer) const override;
		virtual uint16_t EstimateCurrentSerializedSize() const override;

	private:
		std::string m_playerNickname;
	};
}

