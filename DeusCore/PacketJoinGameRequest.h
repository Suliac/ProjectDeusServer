#pragma once
#include "Packet.h"
namespace DeusCore
{
	class PacketJoinGameRequest : public Packet
	{
	public:
		PacketJoinGameRequest();
		~PacketJoinGameRequest();

		void SetGameId(unsigned int value) { m_gameId = value; }
		unsigned int GetGameId() {return m_gameId; }

	protected:
		virtual void OnDeserialize(Buffer512 & buffer) override;
		virtual void OnSerialize(Buffer512 & buffer) const override;
		virtual uint16_t EstimateCurrentSerializedSize() const override;

	private:
		unsigned int m_gameId;
	};
}

