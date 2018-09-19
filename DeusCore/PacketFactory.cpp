#include "PacketFactory.h"

namespace DeusCore
{
	PacketUPtr PacketFactory::CreatePacket(Packet::EMessageType type)
	{
		PacketUPtr p_packet = nullptr;

		switch (type)
		{
		case Packet::Test:
			p_packet = std::make_unique<PacketTest>();
			break;
		case Packet::Ack:
			p_packet = std::make_unique<PacketAck>();
			break;
		case Packet::Connected:
			p_packet = std::make_unique<PacketClientConnected>();
			break;
		case Packet::CreateGameRequest:
			p_packet = std::make_unique<PacketCreateGameRequest>();
			break;
		case Packet::CreateGameAnswer:
			p_packet = std::make_unique<PacketCreateGameAnswer>();
			break;
		case Packet::JoinGameRequest:
			p_packet = std::make_unique<PacketJoinGameRequest>();
			break;
		case Packet::JoinGameAnswer:
			p_packet = std::make_unique<PacketJoinGameAnswer>();
			break;
		case Packet::GetGameRequest:
			p_packet = std::make_unique<PacketGetGamesRequest>();
			break;
		case Packet::GetGameAnswer:
			p_packet = std::make_unique<PacketGetGamesAnswer>();
			break;
		case Packet::LeaveGameRequest:
			p_packet = std::make_unique<PacketLeaveGameRequest>();
			break;
		case Packet::LeaveGameAnswer:
			p_packet = std::make_unique<PacketLeaveGameAnswer>();
			break;
		case Packet::PlayerReady:
			p_packet = std::make_unique<PacketPlayerReady>();
			break;
		case Packet::PlayerNotReady:
			p_packet = std::make_unique<PacketPlayerNotReady>();
			break;
		case Packet::ConnectedUdpAnswer:
			p_packet = std::make_unique<PacketConnectedUdpAnswer>();
			break;
		case Packet::UpdateMovementRequest:
			p_packet = std::make_unique<PacketUpdateMovementRequest>();
			break;
		case Packet::PingAnswer:
			p_packet = std::make_unique<PacketPingAnswer>();
			break;
		case Packet::PingRequest:
			p_packet = std::make_unique<PacketPingRequest>();
			break;
		/*case Packet::ClockSyncRequest:
			p_packet = std::make_unique<PacketSyncClockRequest>();
			break;*/
		default:
			throw DeusSerializationException("Impossible to deserialize object : unknown message type");
		}

		return p_packet;
	}
}
