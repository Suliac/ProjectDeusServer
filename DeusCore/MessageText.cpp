#include "MessageText.h"

namespace DeusNetwork
{
	MessageText::MessageText()
	{
	}

	MessageText::~MessageText()
	{
	}

	void MessageText::OnSerialize(Buffer512 &buffer) const
	{
		SerializeData<ISerializable>(buffer, m_message);
	}

	void MessageText::OnDeserialize(Buffer512& buffer)
	{		
		DeserializeData<ISerializable>(buffer, m_message);
	}
}
