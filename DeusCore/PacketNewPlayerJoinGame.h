#pragma once
#include "Packet.h"
namespace DeusCore
{
	class PacketNewPlayerJoinGame : public Packet
	{
	public:
		PacketNewPlayerJoinGame(std::string nickname, Id idPlayer)
			:PacketNewPlayerJoinGame()
		{
			m_playerNickname = nickname;
			m_playerId = idPlayer;
		}

		PacketNewPlayerJoinGame();
		~PacketNewPlayerJoinGame();

		Id GetPlayerId() const { return m_playerId; }
		void SetPlayerId(Id value) { m_playerId = value; }


		const std::string& GetNickname() const { return m_playerNickname; }
		void SetNickname(const std::string& value) { m_playerNickname = value; }
	protected:
		virtual void OnDeserialize(Buffer512 & buffer) override;
		virtual void OnSerialize(Buffer512 & buffer) const override;
		virtual uint16_t EstimateCurrentSerializedSize() const override;

	private:
		std::string m_playerNickname;
		Id m_playerId;
	};
}

