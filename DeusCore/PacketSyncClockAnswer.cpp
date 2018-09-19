#include "PacketSyncClockAnswer.h"
#include "DeusSerializationException.h"
namespace DeusCore
{
	PacketSyncClockAnswer::PacketSyncClockAnswer()
		: Packet(EMessageType::ClockSyncAnswer)
	{
	}
	
	PacketSyncClockAnswer::~PacketSyncClockAnswer()
	{
	}

	void PacketSyncClockAnswer::OnDeserialize(Buffer512 & buffer)
	{
		throw DeusSerializationException("Don't try to deserialize this");
	}

	void PacketSyncClockAnswer::OnSerialize(Buffer512 & buffer) const
	{
		SerializeData(buffer, m_currentServerTimeStamp);
	}

	uint16_t PacketSyncClockAnswer::EstimateCurrentSerializedSize() const
	{
		return uint16_t(m_currentServerTimeStamp);
	}
}
