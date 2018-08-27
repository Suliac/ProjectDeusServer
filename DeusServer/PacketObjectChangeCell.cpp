#include "PacketObjectChangeCell.h"

namespace DeusServer
{
	PacketObjectChangeCell::PacketObjectChangeCell(Id clientId, const std::shared_ptr<const GameObject> object, Id leftCellId, Id enteredCellId, std::vector<std::shared_ptr<const GameObject>>& objectInCellsLeft, std::vector<std::shared_ptr<const GameObject>>& objectInCellsEntered, bool isInit)
		: DeusCore::Packet(Packet::EMessageType::ObjectChangeCell)
	{
		m_playerConnectionId = clientId;
		m_object = object;
		m_enteredCellId = enteredCellId;
		m_leftCellId = leftCellId;
		m_isInit = isInit;

		m_gameObjLeaving.swap(objectInCellsLeft);
		m_gameObjEntering.swap(objectInCellsEntered);
	}

	PacketObjectChangeCell::PacketObjectChangeCell(Id clientId, const std::shared_ptr<const GameObject> object, Id leftCellId, Id enteredCellId, bool isInit)
		: DeusCore::Packet(Packet::EMessageType::ObjectChangeCell)
	{
		m_playerConnectionId = clientId;
		m_object = object;
		m_enteredCellId = enteredCellId;
		m_leftCellId = leftCellId;
		m_isInit = isInit;
	}


	PacketObjectChangeCell::~PacketObjectChangeCell()
	{
	}

	void PacketObjectChangeCell::OnDeserialize(DeusCore::Buffer512 & buffer)
	{
		throw DeusCore::DeusSerializationException("This message isn't mean to be serialized");
	}

	void PacketObjectChangeCell::OnSerialize(DeusCore::Buffer512 & buffer) const
	{
		throw DeusCore::DeusSerializationException("This message isn't mean to be serialized");
	}

	uint16_t PacketObjectChangeCell::EstimateCurrentSerializedSize() const
	{
		throw DeusCore::DeusSerializationException("This message isn't mean to be serialized");
	}
}
