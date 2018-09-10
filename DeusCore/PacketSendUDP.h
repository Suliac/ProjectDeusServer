#pragma once
#include "Packet.h"
#include <WinSock2.h>
namespace DeusCore
{
	class PacketSendUDP : public Packet
	{
	public:
		PacketSendUDP();
		~PacketSendUDP();

		std::shared_ptr<addrinfo> AddrInfos;
		PacketSPtr PacketToSend;

	protected:
		virtual void OnDeserialize(Buffer512 &buffer)override {}
		virtual void OnSerialize(Buffer512 &buffer) const override {}
		virtual uint16_t EstimateCurrentSerializedSize() const override { return 0; }
	};
}

