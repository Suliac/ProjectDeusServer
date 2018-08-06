#pragma once
#include "Packets.h"
namespace DeusCore
{
	class PacketFactory
	{
	public:
		static PacketUPtr CreatePacket(Packet::EMessageType type);
	};
}

