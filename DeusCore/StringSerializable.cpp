#include "StringSerializable.h"
#include <iterator>

namespace DeusCore
{
	StringSerializable::StringSerializable()
	{
	}

	StringSerializable::~StringSerializable()
	{
	}

	void StringSerializable::OnSerialize(Buffer512 & buffer) const
	{
		// we first serialize the length of the string
		size_t dataSize = length() + 1; // +1 to add the \0 of string
		SerializeData(buffer, dataSize); // SerializeData only for primitive

		//  then we add the string 
		const char* myCurrentText = c_str();
		for (size_t i = 0; i < dataSize; i++)
		{
			SerializeData(buffer, *myCurrentText);
			myCurrentText++;
		}

		//buffer.Insert((const unsigned char*)c_str(), dataSize);
		//memcpy(buffer.data + buffer.index, c_str(), dataSize + 1);
		//buffer.index += dataSize + 1; // +1 to add the \0
	}

	void StringSerializable::OnDeserialize(Buffer512 & buffer)
	{
		// get the size of the string
		size_t dataSize;
		DeserializeData(buffer, dataSize);

		// get the string
		clear();
		std::vector<unsigned char> tmp = buffer.Select(dataSize);
		assign((char *)tmp.data());
		//buffer.index += dataSize;
	}
}
