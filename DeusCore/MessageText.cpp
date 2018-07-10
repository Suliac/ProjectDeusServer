#include "MessageText.h"

namespace DeusNetwork
{
	MessageText::MessageText()
	{
	}

	MessageText::~MessageText()
	{
	}

	void MessageText::Write(Buffer &buffer)
	{
		SerializeData<std::string>(buffer, m_message);
	}

	void MessageText::Read(Buffer &buffer)
	{
		m_message = DeserializeData<std::string>(buffer);
	}
}
