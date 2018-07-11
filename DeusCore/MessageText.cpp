#include "MessageText.h"

namespace DeusNetwork
{
	MessageText::MessageText()
	{
	}

	MessageText::~MessageText()
	{
	}

	void MessageText::Write(Buffer512 &buffer) const
	{
		SerializeData<std::string>(buffer, m_message);
	}

	void MessageText::Read(const Buffer512 &buffer)
	{
		m_message = DeserializeData<std::string>(buffer);
	}
}
