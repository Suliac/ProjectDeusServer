#include "SerializableHealthComponent.h"

namespace DeusServer {
	SerializableHealthComponent::SerializableHealthComponent(Id componentId, GameObjectComponent::EComponentType componentType, const std::shared_ptr<const int> originValue, uint32_t originMs, const std::shared_ptr<const int> destinationValue, uint32_t destinationMs)
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
		
		bool isThereOrigin = false;
		DeserializeData(buffer, isThereOrigin);
		if (isThereOrigin)
		{
			DeserializeData(buffer, *m_originValue);
			DeserializeData(buffer, m_originMs);
		}

		bool isThereDestination = false;
		DeserializeData(buffer, isThereDestination);
		if (isThereDestination)
		{
			DeserializeData(buffer, *m_destinationValue);
			DeserializeData(buffer, m_destinationMs);
		}
	}

	void SerializableHealthComponent::Serialize(DeusCore::Buffer512 & buffer) const
	{
		SerializeData(buffer, m_componentId);

		uint8_t componentType = m_componentType;
		SerializeData(buffer, componentType);

		bool isThereOrigin = m_originValue != nullptr;
		SerializeData(buffer, isThereOrigin);
		if (isThereOrigin)
		{
			SerializeData(buffer, *m_originValue);
			SerializeData(buffer, m_originMs);
		}
		
		bool isThereDestination = m_destinationValue != nullptr;
		SerializeData(buffer, isThereDestination);
		if (isThereDestination)
		{
			SerializeData(buffer, *m_destinationValue);
			SerializeData(buffer, m_destinationMs);
		}
	}

	uint16_t SerializableHealthComponent::EstimateAnswerCurrentSerializedSize() const
	{
		uint16_t originDataSize = m_originValue != nullptr ? sizeof(*m_originValue) + sizeof(m_originMs) : 0;
		uint16_t destinationDataSize = m_destinationValue != nullptr ? sizeof(*m_destinationValue) + sizeof(m_destinationMs) : 0;

		return uint16_t(sizeof(m_componentId) + sizeof(m_componentType)
			+ sizeof(bool) // bool isThereOrigin
			+ originDataSize
			+ sizeof(bool) // bool isThereDestination
			+ destinationDataSize);
	}
}