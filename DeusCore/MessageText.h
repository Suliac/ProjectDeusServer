#pragma once
#include "Message.h"
#include "StringSerializable.h"
#include <string>

namespace DeusNetwork
{
	class MessageText : public IMessage
	{
	public:
		MessageText();
		~MessageText();

		// Write message information into the buffer
		void OnSerialize(Buffer512 &buffer) const override;

		// Put information from the buffer into the message
		void OnDeserialize(Buffer512 &buffer) override;
	
		void SetMessage(const std::string& message) { m_message.assign(message); }
		const std::string& GetTextMessage() { return m_message; };
	private:
		StringSerializable m_message;
	};
}

