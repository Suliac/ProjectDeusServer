#pragma once
#include "ISerializableComponent.h"
#include "GameObjectComponent.h"
namespace DeusServer
{
	template <typename T>
	class SerializableTimelineComponent : public ISerializableComponent
	{
	public:
		SerializableTimelineComponent(Id componentId, GameObjectComponent::EComponentType componentType, T originValue, uint32_t originMs, T destinationValue, uint32_t destinationMs);
		~SerializableTimelineComponent();

	protected:
		Id m_componentId;
		GameObjectComponent::EComponentType m_componentType;

		T m_originValue;
		uint32_t m_originMs;

		T m_destinationValue;
		uint32_t m_destinationMs;
	};

	template<typename T>
	inline SerializableTimelineComponent<T>::SerializableTimelineComponent(Id componentId, GameObjectComponent::EComponentType componentType, T originValue, uint32_t originMs, T destinationValue, uint32_t destinationMs)
	{
		m_componentId = componentId;
		m_componentType = componentType;

		m_originValue = originValue;
		m_originMs = originMs;

		m_destinationValue = destinationValue;
		m_destinationMs = destinationMs;
	}

	template<typename T>
	inline SerializableTimelineComponent<T>::~SerializableTimelineComponent()
	{
	}
}

