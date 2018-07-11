#include "StringSerializable.h"
#include <iterator>

namespace DeusNetwork
{
	StringSerializable::StringSerializable()
	{
	}

	StringSerializable::~StringSerializable()
	{
	}

	void StringSerializable::Serialize(Buffer512 & buffer) const
	{
		// we first serialize the length of the string
		size_t dataSize = length();
		SerializeData(buffer, dataSize);

		//  then we add the string 
		memcpy(buffer.data + buffer.index, c_str(), dataSize + 1);
		buffer.index += dataSize + 1; // +1 to add the \0
	}

	void StringSerializable::Deserialize(Buffer512 & buffer)
	{
		// get the size of the string
		size_t dataSize;
		DeserializeData(buffer, dataSize);

		// get the string
		clear();

		assign((const char*)(buffer.data + buffer.index));

		buffer.index += dataSize;
	}
}
