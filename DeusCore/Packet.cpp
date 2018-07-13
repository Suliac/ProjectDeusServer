#include "Packet.h"
#include "PacketTest.h"
namespace DeusNetwork
{
	Packet::Packet() : Packet(Packet::EMessageType::MessageTest)
	{
	}


	Packet::~Packet()
	{
	}

	Packet* Packet::Deserialize(Buffer512 & buffer)
	{
		Packet* p_monPaquet = nullptr;
		buffer.SetIndex(0);

		uint8_t id;
		Packet::DeserializeData(buffer, id);

		switch (id)
		{
		case Packet::MessageTest:
			p_monPaquet = new PacketTest();
			break;
		default:
			break;
		}

		if (p_monPaquet == nullptr)
			throw std::exception("Pointer null");

		p_monPaquet->OnDeserialize(buffer);
		return p_monPaquet;
	}

	void Packet::Serialize(Buffer512 & buffer, const Packet & paquet)
	{
		buffer.SetIndex(0);

		uint8_t id = paquet.GetID();
		paquet.SerializeData(buffer, id);

		paquet.OnSerialize(buffer);
	}
}
