#pragma once
#include "Message.h"
#include <string>

namespace DeusNetwork
{
	class MessageText : public Message
	{
	public:
		MessageText();
		~MessageText();

		void Write(Buffer &buffer) override;
		void Read(Buffer &buffer) override;
	
		void SetMessage(std::string message) { m_message = message; }
		std::string GetMessage() { return m_message; };
	private:
		std::string m_message;
	};
}

