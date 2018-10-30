#pragma once
#include "SerializableTimelineComponent.h"

#include "DeusCore/Skill.h"
namespace DeusServer
{
	class SerializableSkillComponent : public SerializableTimelineComponent<DeusCore::SkillInfos>
	{
	public:
		SerializableSkillComponent(Id componentId, GameObjectComponent::EComponentType componentType, const std::shared_ptr<const DeusCore::SkillInfos> originValue, uint32_t originMs, const std::shared_ptr<const DeusCore::SkillInfos> destinationValue, uint32_t destinationMs);
		~SerializableSkillComponent();

	protected:
		virtual void Deserialize(DeusCore::Buffer512 & buffer) override;
		virtual void Serialize(DeusCore::Buffer512 & buffer) const override;
		virtual uint16_t EstimateAnswerCurrentSerializedSize() const override;
	};
}

