#pragma once
#include "ISerializableComponent.h"
#include "GameObjectComponent.h"
namespace DeusServer
{
	template <typename T>
	class SerializableTimelineComponent : public ISerializableComponent
	{
	public:
		SerializableTimelineComponent(Id componentId, GameObjectComponent::EComponentType componentType, const std::shared_ptr<const T> p_originValue, uint32_t originMs, const std::shared_ptr<const T> p_destinationValue, uint32_t destinationMs);
		~SerializableTimelineComponent();

	protected:
		Id m_componentId;
		GameObjectComponent::EComponentType m_componentType;

		std::shared_ptr<T> m_originValue;
		uint32_t m_originMs;

		std::shared_ptr<T> m_destinationValue;
		uint32_t m_destinationMs;
	};


	template<typename T>
	inline SerializableTimelineComponent<T>::SerializableTimelineComponent(Id componentId, GameObjectComponent::EComponentType componentType, const std::shared_ptr<const T> p_originValue, uint32_t originMs, const std::shared_ptr<const T> p_destinationValue, uint32_t destinationMs)
	{
		m_componentId = componentId;
		m_componentType = componentType;

		if (p_originValue)
		{
			m_originValue = std::make_shared<T>(*p_originValue);
			m_originMs = originMs;
		}

		if (p_destinationValue)
		{
			m_destinationValue = std::make_shared<T>(*p_destinationValue);
			m_destinationMs = destinationMs;
		}
	}

	template<typename T>
	inline SerializableTimelineComponent<T>::~SerializableTimelineComponent()
	{
	}
}

