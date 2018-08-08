#pragma once
#include "DeusCore/Packet.h"
#include "GameObject.h"

namespace DeusServer
{
	class PacketObjectChangeCell : public DeusCore::Packet
	{
	public:
		PacketObjectChangeCell(Id clientId, const std::shared_ptr<const GameObject> object, Id leftCellId, Id enteredCellId, std::vector<std::shared_ptr<const GameObject>>& objectInCellsLeft, std::vector<std::shared_ptr<const GameObject>>& objectInCellsEntered, bool isInit = false);
		PacketObjectChangeCell(Id clientId, const std::shared_ptr<const GameObject> object, Id leftCellId, Id enteredCellId, bool isInit = false);
		~PacketObjectChangeCell();

		Id GetClientId() const { return m_playerConnectionId; }
		const std::shared_ptr<const GameObject> GetGameObject() const { return m_object; }
		Id GetLeftCellId() const { return m_leftCellId; }
		Id GetEnteredCellId() const { return m_enteredCellId; }
		bool IsInit() const { return m_isInit; }

		const std::vector<std::shared_ptr<const GameObject>>& GetObjectInLeavingCells() const { return m_gameObjLeaving; }
		const std::vector<std::shared_ptr<const GameObject>>& GetObjectInEnteringCells()const { return m_gameObjEntering; }
	protected:
		virtual void OnDeserialize(DeusCore::Buffer512 & buffer) override;
		virtual void OnSerialize(DeusCore::Buffer512 & buffer) const override;
		virtual uint16_t EstimateCurrentSerializedSize() const override;

	private:
		Id m_playerConnectionId;
		std::shared_ptr<const GameObject> m_object;
		Id m_leftCellId;
		Id m_enteredCellId;
		bool m_isInit;

		std::vector<std::shared_ptr<const GameObject>> m_gameObjLeaving;
		std::vector<std::shared_ptr<const GameObject>> m_gameObjEntering;
	};
}
