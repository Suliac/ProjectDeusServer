#pragma once
#include "Buffer.h"
#include "DeusSerializationException.h"
#include <memory>
namespace DeusCore
{
	class ISerializable
	{
	public:
		// Update buffer index to 'offset' param then call OnSerialize method
		virtual void Deserialize(Buffer512 &buffer) = 0;
		
		// Update buffer index to 'offset' param then call OnDeserialize method
		virtual void Serialize(Buffer512 &buffer) const = 0;

		virtual uint16_t EstimateAnswerCurrentSerializedSize() const = 0;
	protected:
		template<typename T>
		static void SerializeData(Buffer512& buffer, const T& value);

		template<typename T>
		static void DeserializeData(Buffer512& buffer, T& value);
	};

#pragma region Deserialization
	template<typename T>
	inline void ISerializable::DeserializeData(Buffer512& buffer, T& value)
	{
		static_assert(sizeof(T) <= 8, "Size higher than 8 bytes"); // only work for small types

		unsigned char tmp[sizeof(T)];
		buffer.Select(tmp, sizeof(T));

		value = T();

		// we decode the big endian
		for (size_t i = 0; i < sizeof(T); i++)
			value |= tmp[i] << (8 * ((sizeof(T) - 1) - i));

	}

#pragma endregion


#pragma region Serialization
	template<typename T>
	inline void ISerializable::SerializeData(Buffer512& buffer, const T& value) {
		static_assert(sizeof(T) <= 8, "Size higher than 8 bytes"); // only work for small types
		unsigned char datas[sizeof(T)];

		size_t size = sizeof(T);

		// we encode a big endian value as shifting involve 'auto' convert to big endian
		// then we take the strongest byte as first buffer byte, and so on
		// src : https://stackoverflow.com/questions/7184789/does-bit-shift-depend-on-endianness
		for (size_t i = 0; i < size; i++)
			datas[i] = value >> (8 * ((size - 1) - i));

		buffer.Insert(datas, sizeof(T));
	}

#pragma endregion
}


