#include "SerializableHealthComponent.h"

namespace DeusServer {
	SerializableHealthComponent::SerializableHealthComponent(Id componentId, GameObjectComponent::EComponentType componentType, int originValue, uint32_t originMs, int destinationValue, uint32_t destinationMs)
		: SerializableTimelineComponent<int>(componentId, componentType, originValue, originMs, destinationValue, destinationMs)
	{
	}
	
	SerializableHealthComponent::~SerializableHealthComponent()
	{

	}

	void SerializableHealthComponent::Deserialize(DeusCore::Buffer512 & buffer)
	{
		DeserializeData(buffer, m_componentId);

		uint8_t componentType = m_componentType;
		DeserializeData(buffer, componentType);
		m_componentType = (GameObjectComponent::EComponentType)componentType;

		DeserializeData(buffer, m_originValue);
		DeserializeData(buffer, m_originMs);

		DeserializeData(buffer, m_destinationValue);
		DeserializeData(buffer, m_destinationMs);
	}

	void SerializableHealthComponent::Serialize(DeusCore::Buffer512 & buffer) const
	{
		SerializeData(buffer, m_componentId);

		uint8_t componentType = m_componentType;
		SerializeData(buffer, componentType);

		SerializeData(buffer, m_originValue);
		SerializeData(buffer, m_originMs);

		SerializeData(buffer, m_destinationValue);
		SerializeData(buffer, m_destinationMs);
	}

	uint16_t SerializableHealthComponent::EstimateAnswerCurrentSerializedSize() const
	{
		return uint16_t(sizeof(m_componentId) + sizeof(m_componentType)
			+ sizeof(m_originValue)+ sizeof(m_originMs)
			+ sizeof(m_destinationValue) + sizeof(m_destinationMs));
	}
}