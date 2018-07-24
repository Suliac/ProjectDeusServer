#pragma once
#include "DeusCore/Packet.h"
#include "DeusClient.h"

namespace DeusServer
{
	class PacketNewPlayer : public DeusCore::Packet
	{
	public:
		PacketNewPlayer(std::shared_ptr<DeusClient> clientConnection);
		~PacketNewPlayer();

		std::shared_ptr<DeusClient> GetConnection() { return m_clientConnection; }

	protected:
		virtual void OnDeserialize(DeusCore::Buffer512 & buffer) override;
		virtual void OnSerialize(DeusCore::Buffer512 & buffer) const override;
		virtual uint16_t EstimateCurrentSerializedSize() const override;

	private:
		DeusClientSPtr m_clientConnection;
	};
}

