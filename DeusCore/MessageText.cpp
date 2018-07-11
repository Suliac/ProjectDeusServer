#include "MessageText.h"

namespace DeusNetwork
{
	MessageText::MessageText()
	{
	}

	MessageText::~MessageText()
	{
	}

	void MessageText::Serialize(Buffer512 &buffer) const
	{
		SerializeData<ISerializable>(buffer, m_message);
	}

	void MessageText::Deserialize(Buffer512& buffer)
	{		
		DeserializeData<ISerializable>(buffer, m_message);
	}
}
