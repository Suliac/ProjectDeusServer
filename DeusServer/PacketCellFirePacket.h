#pragma once
#include "DeusCore/Packet.h"
namespace DeusServer
{
	class CellFirePacket : public DeusCore::Packet
	{
	public:
		CellFirePacket(Id cellId, Id objectId, std::unique_ptr<DeusCore::Packet>&& packetFired);
		CellFirePacket();
		~CellFirePacket();

		Id GetCellId() const { return m_cellId; }
		Id GetObjectId() const { return m_objectId; }
		std::unique_ptr<DeusCore::Packet> ExtractPacket() { return std::move(m_packet); }

	protected:
		virtual void OnDeserialize(DeusCore::Buffer512 & buffer) override;
		virtual void OnSerialize(DeusCore::Buffer512 & buffer) const override;
		virtual uint16_t EstimateCurrentSerializedSize() const override;

	private:
		std::unique_ptr<DeusCore::Packet> m_packet;
		Id m_cellId;
		Id m_objectId;
	};
}

