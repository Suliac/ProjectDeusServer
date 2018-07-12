#pragma once
#include "Buffer.h"
#include <assert.h>

namespace DeusNetwork
{
	class ISerializable
	{
	public:
		// Update buffer index to 'offset' param then call OnSerialize method
		void Serialize(Buffer512 &buffer, size_t offset = 0) const;
		
		// Update buffer index to 'offset' param then call OnDeserialize method
		void Deserialize(Buffer512 &buffer, size_t offset = 0);

	protected:
		// Define specific serialization method for each children
		virtual void OnSerialize(Buffer512 &buffer) const = 0;

		// Define specific deserialization method for each children
		virtual void OnDeserialize(Buffer512 &buffer) = 0;

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
		unsigned char tmp[sizeof(T)];
		buffer.Select(tmp, sizeof(T));

#ifdef BIG_ENDIAN
		value = bswap(*(tmp));
#else // #ifdef BIG_ENDIAN
		value = *(tmp);
#endif // #ifdef BIG_ENDIAN
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
		unsigned char datas[sizeof(T)];

#ifdef BIG_ENDIAN
		*(datas) = bswap(value);
#else // #ifdef BIG_ENDIAN
		*(datas) = value;
#endif // #ifdef BIG_ENDIAN

		buffer.Insert(datas, sizeof(T));
	}

	template<>
	inline void ISerializable::SerializeData(Buffer512& buffer, const ISerializable& value) const {
		value.Serialize(buffer);
	}

#pragma endregion
}

