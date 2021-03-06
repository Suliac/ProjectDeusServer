#include "PacketObjectEnter.h"
#include "DeusCore/Logger.h"
namespace DeusServer
{
	PacketObjectEnter::PacketObjectEnter(Id objectId, GameObject::EObjectType objectType, bool isLocalPlayer, std::vector<std::shared_ptr<ISerializableComponent>>& components, Id playerConnectedId)
		: DeusCore::Packet(Packet::EMessageType::ObjectEnter)
	{
		m_objectId = objectId;
		m_objectType = objectType;
		m_isLocalPlayer = isLocalPlayer;
		m_playerId = playerConnectedId;

		m_components.swap(components);
	}

	PacketObjectEnter::~PacketObjectEnter()
	{
	}

	void PacketObjectEnter::OnDeserialize(DeusCore::Buffer512 & buffer)
	{
		/*DeserializeData(buffer, m_objectId);

		uint8_t objectType = m_objectType;
		DeserializeData(buffer, objectType);
		m_objectType = (GameObject::EObjectType)objectType;

		DeserializeData(buffer, m_isLocalPlayer);

		uint8_t numberComponents = 0;
		DeserializeData(buffer, numberComponents);

		for (int i = 0; i < numberComponents; i++)
		{

		}*/
		throw DeusCore::DeusSerializationException("Don't deserialize this, it's just Server -> Client packet");
	}

	void PacketObjectEnter::OnSerialize(DeusCore::Buffer512 & buffer) const
	{
		SerializeData(buffer, m_objectId);

		uint8_t objectType = m_objectType;
		SerializeData(buffer, objectType);

		SerializeData(buffer, m_isLocalPlayer);
		SerializeData(buffer, m_playerId);

		uint8_t componentsNumber = (uint8_t)m_components.size();
		SerializeData(buffer, componentsNumber);

		for (int i = 0; i < componentsNumber; i++)
		{
			SerializeData<DeusCore::ISerializable>(buffer, *m_components[i]);
		}

		//DeusCore::Logger::Instance()->Log("DEBUG", "Serializez ObjectEnter");
	}

	uint16_t PacketObjectEnter::EstimateCurrentSerializedSize() const
	{
		uint16_t sizeComponents = 1; // 1 for the size of compoenents' size
		for (size_t i = 0; i < m_components.size(); i++)
			sizeComponents += m_components[i]->EstimateAnswerCurrentSerializedSize();

		return uint16_t(sizeof(m_objectId) + sizeof(m_objectType) + sizeof(m_isLocalPlayer) +sizeof(m_playerId) + sizeComponents);
	}
}
