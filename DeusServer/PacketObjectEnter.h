#pragma once
#include "DeusCore/Packet.h"
#include "GameObject.h"
#include "ISerializableComponent.h"

namespace DeusServer
{
	using SerializableComponents = std::vector<std::shared_ptr<ISerializableComponent>>;
	class PacketObjectEnter : public DeusCore::Packet
	{
	public:
		PacketObjectEnter(Id objectId, GameObject::EObjectType objectType, bool isLocalPlayer, std::vector<std::shared_ptr<ISerializableComponent>>& components);
		~PacketObjectEnter();

	protected:
		virtual void OnDeserialize(DeusCore::Buffer512 & buffer) override;
		virtual void OnSerialize(DeusCore::Buffer512 & buffer) const override;
		virtual uint16_t EstimateCurrentSerializedSize() const override;

	private:
		Id m_objectId;
		GameObject::EObjectType m_objectType;
		bool m_isLocalPlayer;

		SerializableComponents m_components;
	};
}