#pragma once
#include "SerializableTimelineComponent.h"
#include "DeusCore/DeusVector2.h"
namespace DeusServer
{
	class SerializablePositionComponent:  public SerializableTimelineComponent<DeusCore::DeusVector2>
	{
	public:
		SerializablePositionComponent(Id componentId, GameObjectComponent::EComponentType componentType, const std::shared_ptr<const DeusCore::DeusVector2> originValue, uint32_t originMs, const std::shared_ptr<const DeusCore::DeusVector2> destinationValue, uint32_t destinationMs);
		~SerializablePositionComponent();

		virtual void Deserialize(DeusCore::Buffer512 & buffer) override;
		virtual void Serialize(DeusCore::Buffer512 & buffer) const override;
		virtual uint16_t EstimateAnswerCurrentSerializedSize() const override;
	};
}

