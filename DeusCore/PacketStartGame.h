#pragma once
#include "Packet.h"
namespace DeusCore
{
	class PacketStartGame : public Packet
	{
	public:
		PacketStartGame();
		~PacketStartGame();

		void AddPlayerConnectionId(uint32_t id) { m_playersConnectionId.push_back(id); }
		const std::vector<uint32_t>& GetPlayerConnectionId() const { return m_playersConnectionId; }

	protected:
		virtual void OnDeserialize(Buffer512 & buffer) override;
		virtual void OnSerialize(Buffer512 & buffer) const override;
		virtual uint16_t EstimateCurrentSerializedSize() const override;

	private:
		std::vector<uint32_t> m_playersConnectionId;
	};
}


