#pragma once
#include "Buffer.h"
namespace DeusNetwork
{
	class Packet
	{
	public:
		enum EMessageType : uint8_t
		{
			MessageError = 0x0,
			MessageTest = 0x1
		};

		Packet();
		~Packet();

		static Packet* Deserialize(Buffer512 &buffer);
		static void Serialize(Buffer512 &buffer, const Packet &paquet);

		uint8_t GetID() const { return m_id; }
		Packet(EMessageType messageType) { m_id = messageType; }

	protected:
		virtual void OnDeserialize(Buffer512 &buffer) {};
		virtual void OnSerialize(Buffer512 &buffer) const {};

		template<typename T>
		static void SerializeData(Buffer512& buffer, const T& value);

		template<typename T>
		static void DeserializeData(Buffer512& buffer, T& value);

	private:
		uint8_t m_id = 0x0;
	};

#pragma region Deserialization
	template<typename T>
	inline void Packet::DeserializeData(Buffer512& buffer, T& value)
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

#pragma endregion


#pragma region Serialization
	template<typename T>
	inline void Packet::SerializeData(Buffer512& buffer, const T& value) {
		static_assert(sizeof(T) <= 8, "Size higher than 8 bytes"); // only work for small types
		unsigned char datas[sizeof(T)];

#ifdef BIG_ENDIAN
		*(datas) = bswap(value);
#else // #ifdef BIG_ENDIAN
		*(datas) = value;
#endif // #ifdef BIG_ENDIAN

		buffer.Insert(datas, sizeof(T));
	}

#pragma endregion
}

