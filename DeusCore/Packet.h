#pragma once
#include "Buffer.h"
#include <memory>
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
		Packet(EMessageType messageType) { m_id = messageType; }

		~Packet();

		// Entry point to deserialize packets :
		// - Set buffer's index to 'bufferIndexOffset' param (0 by default)
		// - Deserialize custom headers (message id type)
		// - Call specific deserialization method for right message type with OnDeserialize which has to be override by the childrens packets
		static std::unique_ptr<Packet> Deserialize(Buffer512 &buffer, const size_t bufferIndexOffset = 0);
				
		// Entry point to serialize packets :
		// - Set buffer's index to 'bufferIndexOffset' param (0 by default)
		// - Serialize custom headers (message id type)
		// - Call specific deserialization method with OnDeserialize which has to be override by the childrens packets
		static void Serialize(Buffer512 &buffer, const Packet &paquet, const size_t bufferIndexOffset = 0);

		
		uint8_t GetID() const { return m_id; }

		uint16_t GetSerializedSize() const { return m_serializedSize; }
		void SetSerializedSize(uint16_t serializedSize) { m_serializedSize = serializedSize; }
		
	protected:
		template<typename T>
		static void SerializeData(Buffer512& buffer, const T& value);

		template<typename T>
		static void DeserializeData(Buffer512& buffer, T& value);

		// Specific method to deserialize specific packets, return serialized size
		virtual void OnDeserialize(Buffer512 &buffer) = 0;

		// Specific method to serialize specific packets
		virtual void OnSerialize(Buffer512 &buffer) const = 0;
		
		
		// Specific method to get the serialize size of packets
		virtual int16_t EstimateCurrentSerializedSize() const = 0;

	private:
		
		uint8_t m_id = 0x0;
		uint16_t m_serializedSize = 0x0;
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

	using PacketUPtr = std::unique_ptr<Packet>;
}

