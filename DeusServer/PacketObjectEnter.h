#pragma once
#include "DeusCore/Packet.h"
#include "GameObject.h"

namespace DeusServer
{
	class PacketObjectEnter : public DeusCore::Packet
	{
	public:
		PacketObjectEnter(uint32_t objectId, GameObject::EObjectType objectType);
		~PacketObjectEnter();

	protected:
		virtual void OnDeserialize(DeusCore::Buffer512 & buffer) override;
		virtual void OnSerialize(DeusCore::Buffer512 & buffer) const override;
		virtual uint16_t EstimateCurrentSerializedSize() const override;

	private:
		uint32_t m_objectId;
		GameObject::EObjectType m_objectType;
	};
}