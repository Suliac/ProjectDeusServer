#include "SerializablePositionComponent.h"

namespace DeusServer
{
	SerializablePositionComponent::SerializablePositionComponent(Id componentId, GameObjectComponent::EComponentType componentType, const std::shared_ptr<const DeusCore::DeusVector2> originValue, uint32_t originMs, const std::shared_ptr<const DeusCore::DeusVector2> destinationValue, uint32_t destinationMs)
		: SerializableTimelineComponent<DeusCore::DeusVector2>(componentId, componentType, originValue, originMs, destinationValue, destinationMs)
	{
	}


	SerializablePositionComponent::~SerializablePositionComponent()
	{

	}

	void SerializablePositionComponent::Deserialize(DeusCore::Buffer512 & buffer)
	{
		DeserializeData(buffer, m_componentId);

		uint8_t componentType = m_componentType;
		DeserializeData(buffer, componentType);
		m_componentType = (GameObjectComponent::EComponentType)componentType;
		
		DeserializeData<DeusCore::ISerializable>(buffer, *m_originValue);
		DeserializeData(buffer, m_originMs);

		bool isThereDestination = false;
		DeserializeData(buffer, isThereDestination);
		if (isThereDestination)
		{
			DeserializeData<DeusCore::ISerializable>(buffer, *m_destinationValue);
			DeserializeData(buffer, m_destinationMs);
		}
	}

	void SerializablePositionComponent::Serialize(DeusCore::Buffer512 & buffer) const
	{
		SerializeData(buffer, m_componentId);

		uint8_t componentType = m_componentType;
		SerializeData(buffer, componentType);

		SerializeData<DeusCore::ISerializable>(buffer, *m_originValue);
		SerializeData(buffer, m_originMs);

		bool isThereDestination = false;
		SerializeData(buffer, isThereDestination);
		if (m_destinationValue != nullptr)
		{
			SerializeData<DeusCore::ISerializable>(buffer, *m_destinationValue);
			SerializeData(buffer, m_destinationMs);
		}
	}

	uint16_t SerializablePositionComponent::EstimateAnswerCurrentSerializedSize() const
	{
		uint16_t originDataSize = m_originValue != nullptr ? m_originValue->EstimateAnswerCurrentSerializedSize() + sizeof(m_originMs) : 0;
		uint16_t destinationDataSize = m_destinationValue != nullptr ? m_destinationValue->EstimateAnswerCurrentSerializedSize() + sizeof(m_destinationMs) : 0;

		return uint16_t(sizeof(m_componentId) + sizeof(m_componentType)
			+ originDataSize 
			+ sizeof(bool) // bool isThereDestination
			+ destinationDataSize);
	}
}
