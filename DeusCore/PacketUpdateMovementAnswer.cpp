#include "PacketUpdateMovementAnswer.h"

namespace DeusCore
{	
	PacketUpdateMovementAnswer::PacketUpdateMovementAnswer(Id objectId, Id componentId, DeusVector2 origin, unsigned long originMs, DeusVector2 dest, unsigned long destMs)
		: DeusCore::PacketAnswer(Packet::EMessageType::UpdateMovementAnswer)
	{
		m_objectId = objectId;
		m_componentId = componentId;

		m_originPos = origin;
		m_originPosMs = originMs;

		m_destPos = dest;
		m_destPosMs = destMs;
	}

	PacketUpdateMovementAnswer::PacketUpdateMovementAnswer() 
		: DeusCore::PacketAnswer(Packet::EMessageType::UpdateMovementAnswer)
	{
	}

	PacketUpdateMovementAnswer::~PacketUpdateMovementAnswer()
	{
	}

	void PacketUpdateMovementAnswer::OnAnswerDeserialize(DeusCore::Buffer512 & buffer)
	{
		DeserializeData(buffer, m_objectId);
		DeserializeData(buffer, m_componentId);

		DeserializeData<ISerializable>(buffer, m_originPos);
		DeserializeData(buffer, m_originPosMs);

		DeserializeData<ISerializable>(buffer, m_destPos);
		DeserializeData(buffer, m_destPosMs);
	}

	void PacketUpdateMovementAnswer::OnAnswerSerialize(DeusCore::Buffer512 & buffer) const
	{
		SerializeData(buffer, m_objectId);
		SerializeData(buffer, m_componentId);

		SerializeData<ISerializable>(buffer, m_originPos);
		SerializeData(buffer, m_originPosMs);

		SerializeData<ISerializable>(buffer, m_destPos);
		SerializeData(buffer, m_destPosMs);
	}

	uint16_t PacketUpdateMovementAnswer::EstimateAnswerCurrentSerializedSize() const
	{
		return uint16_t(sizeof(m_objectId) + sizeof(m_componentId) 
			+ (m_originPos.EstimateAnswerCurrentSerializedSize() + sizeof(m_originPosMs)) 
			+ (m_destPos.EstimateAnswerCurrentSerializedSize() + sizeof(m_destPosMs)));
	}
}
