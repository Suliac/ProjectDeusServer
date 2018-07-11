#pragma once
#include "Buffer.h"
#include <assert.h>

namespace DeusNetwork
{
	class ISerializable
	{
	public:

		virtual void Serialize(Buffer512 &buffer) const = 0;
		virtual void Deserialize(Buffer512 &buffer) = 0;

	protected:

		template<typename T>
		void SerializeData(Buffer512& buffer, const T& value) const;

		template<typename T>
		void DeserializeData(Buffer512& buffer, T& value) const;
	};

#pragma region Deserialization
	template<typename T>
	inline void ISerializable::DeserializeData(Buffer512& buffer, T& value) const
	{
		static_assert(sizeof(T) <= 8, "Size higher than 8 bytes"); // only work for small types
		assert(buffer.index + sizeof(T) <= buffer.size); // can we read the nextbytes ?
		
#ifdef BIG_ENDIAN
		value = bswap(*((unsigned char*)(buffer.data + buffer.index)));
#else // #ifdef BIG_ENDIAN
		value = *((unsigned char*)(buffer.data + buffer.index));
#endif // #ifdef BIG_ENDIAN

		buffer.index += sizeof(T);
	}

	template<>
	inline void ISerializable::DeserializeData(Buffer512& buffer, ISerializable& value) const
	{
		value.Deserialize(buffer);
	}
#pragma endregion


#pragma region Serialization
	template<typename T>
	inline void ISerializable::SerializeData(Buffer512& buffer, const T& value) const {
		static_assert(sizeof(T) <= 8, "Size higher than 8 bytes"); // only work for small types
		assert(buffer.index + sizeof(T) <= buffer.size);

#ifdef BIG_ENDIAN
		*((unsigned char*)(buffer.data + buffer.index)) = bswap(value);
#else // #ifdef BIG_ENDIAN
		*((unsigned char*)(buffer.data + buffer.index)) = value;
#endif // #ifdef BIG_ENDIAN

		buffer.index += sizeof(T);
	}

	template<>
	inline void ISerializable::SerializeData(Buffer512& buffer, const ISerializable& value) const {
		value.Serialize(buffer);
	}

#pragma endregion
}

