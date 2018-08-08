#pragma once
#include "DeusCore/Packet.h"
#include "GameObject.h"

namespace DeusServer
{
	class PacketObjectChangeCell : public DeusCore::Packet
	{
	public:
		PacketObjectChangeCell(Id objectId, Id clientId, GameObject::EObjectType objectType, Id leftCellId, Id enteredCellId, std::vector<std::shared_ptr<GameObject>>& objectInCellsLeft, std::vector<std::shared_ptr<GameObject>>& objectInCellsEntered, bool isInit = false);
		PacketObjectChangeCell(Id objectId, Id clientId, GameObject::EObjectType objectType, Id leftCellId, Id enteredCellId, bool isInit = false);
		~PacketObjectChangeCell();

		Id GetObjectId() const { return m_objectId; }
		Id GetClientId() const { return m_playerConnectionId; }
		GameObject::EObjectType GetObjectType() const { return m_objectType; }
		Id GetLeftCellId() const { return m_leftCellId; }
		Id GetEnteredCellId() const { return m_enteredCellId; }
		bool IsInit() const { return m_isInit; }
	protected:
		virtual void OnDeserialize(DeusCore::Buffer512 & buffer) override;
		virtual void OnSerialize(DeusCore::Buffer512 & buffer) const override;
		virtual uint16_t EstimateCurrentSerializedSize() const override;

	private:
		Id m_objectId;
		Id m_playerConnectionId;
		GameObject::EObjectType m_objectType;
		Id m_leftCellId;
		Id m_enteredCellId;
		bool m_isInit;

		std::vector<std::shared_ptr<GameObject>> m_gameObjLeaving;
		std::vector<std::shared_ptr<GameObject>> m_gameObjEntering;
	};
}
