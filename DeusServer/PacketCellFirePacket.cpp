#include "PacketCellFirePacket.h"

namespace DeusServer
{
	CellFirePacket::CellFirePacket(Id cellId, Id objectId, std::shared_ptr<DeusCore::Packet>&& packetFired)
		: DeusCore::Packet(DeusCore::Packet::EMessageType::CellFirePacket)
	{
		m_cellId = cellId;
		m_objectId = objectId;
		m_packet = std::move(packetFired);
	}

	CellFirePacket::CellFirePacket()
		: DeusCore::Packet(DeusCore::Packet::EMessageType::CellFirePacket)
	{
	}

	CellFirePacket::~CellFirePacket()
	{
	}

	void CellFirePacket::OnDeserialize(DeusCore::Buffer512 & buffer)
	{
		throw DeusCore::DeusSerializationException("This message isn't mean to be sent on the network");
	}

	void CellFirePacket::OnSerialize(DeusCore::Buffer512 & buffer) const
	{
		throw DeusCore::DeusSerializationException("This message isn't mean to be sent on the network");
	}

	uint16_t CellFirePacket::EstimateCurrentSerializedSize() const
	{
		throw DeusCore::DeusSerializationException("This message isn't mean to be sent on the network");
	}
}
