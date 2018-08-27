#pragma once
#include "SerializableTimelineComponent.h"
#include "DeusCore/DeusVector2.h"
namespace DeusServer
{
	class SerializableHealthComponent : public SerializableTimelineComponent<int>
	{
	public:
		SerializableHealthComponent(Id componentId, GameObjectComponent::EComponentType componentType, int originValue, uint32_t originMs, int destinationValue, uint32_t destinationMs);
		~SerializableHealthComponent();

		virtual void Deserialize(DeusCore::Buffer512 & buffer) override;
		virtual void Serialize(DeusCore::Buffer512 & buffer) const override;
		virtual uint16_t EstimateAnswerCurrentSerializedSize() const override;
	};
}

