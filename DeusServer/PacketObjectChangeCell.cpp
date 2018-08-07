#include "PacketObjectChangeCell.h"

namespace DeusServer
{
	PacketObjectChangeCell::PacketObjectChangeCell(Id objectId, Id clientId, GameObject::EObjectType objectType, Id leftCellId, Id enteredCellId, std::vector<std::shared_ptr<GameObject>>& objectInCellsLeft, std::vector<std::shared_ptr<GameObject>>& objectInCellsEntered)
		: DeusCore::Packet(Packet::EMessageType::ObjectChangeCell)
	{
		m_objectId = objectId;
		m_playerConnectionId = clientId;
		m_objectType = objectType;
		m_enteredCellId = enteredCellId;
		m_leftCellId = leftCellId;

		m_gameObjLeaving.swap(objectInCellsLeft);
		m_gameObjEntering.swap(objectInCellsEntered);
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
