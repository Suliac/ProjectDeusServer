#include "Message.h"

#include <assert.h>
#include <stdlib.h>
#include <string>
#include <cstdint>

namespace DeusNetwork
{
	template<typename T>
	void Message::SerializeData(Buffer512& buffer, const T& value) const {
		assert(sizeof(T) <= 8); // only work for small types
		assert(buffer.index + sizeof(T) <= buffer.size);

#ifdef BIG_ENDIAN
		*((unsigned char*)(buffer.data + buffer.index)) = bswap(value);
#else // #ifdef BIG_ENDIAN
		*((unsigned char*)(buffer.data + buffer.index)) = value;
#endif // #ifdef BIG_ENDIAN

		buffer.index += sizeof(T);
	}

	template<>
	void Message::SerializeData<std::string>(Buffer512& buffer, const std::string& value) const{
		const char* charValue = value.c_str();
		for (int i = 0; i < value.length(); i++)
		{
			SerializeData(buffer, *charValue);
			charValue++;
		}
	}

	template<typename T>
	T Message::DeserializeData(const Buffer512& buffer) const
	{
		assert(sizeof(T) <= 8); // only work for small types
		assert(buffer.index + sizeof(T) <= buffer.size); // can we read the nextbytes ?

		T value;

#ifdef BIG_ENDIAN
		value = bswap(*((uint32_t*)(buffer.data + buffer.index)));
#else // #ifdef BIG_ENDIAN
		value = *((uint32_t*)(buffer.data + buffer.index));
#endif // #ifdef BIG_ENDIAN

		buffer.index += sizeof(T);
		return value;
	}

}
