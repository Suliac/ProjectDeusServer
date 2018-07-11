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

		void Write(Buffer512 &buffer) const override;
		void Read(const Buffer512 &buffer) override;
	
		void SetMessage(const std::string& message) { m_message = message; }
		const std::string& GetMessage() { return m_message; };
	private:
		std::string m_message;
	};
}

