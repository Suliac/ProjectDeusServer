#pragma once
#include "Packet.h"
#include <string>
namespace DeusCore
{
	class PacketTest : public Packet
	{
	public:
		PacketTest();
		~PacketTest();

		const std::string& GetTextMessage() const { return m_message; }
		void SetMessage(const std::string& value) { m_message = value; }

	protected:

		virtual void OnDeserialize(Buffer512 & buffer) override;
		virtual void OnSerialize(Buffer512 & buffer) const override;
		virtual uint16_t EstimateCurrentSerializedSize() const override;

	private:
		std::string m_message;

	};
}

