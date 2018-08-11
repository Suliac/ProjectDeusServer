#pragma once
#include "ISerializable.h"

#include <memory>
#include <mutex>

using Id = uint32_t;
namespace DeusCore
{
	class Packet
	{
	public:
		static Id nextId;

		enum EMessageType : uint8_t
		{
			Error					= 0,
			Test					= 1,

			// General
			Disconnect				= 2,
			Ack						= 3,
			Connected				= 4,
			ConnectedUdpAnswer		= 5,

			// Game management
			CreateGameRequest		= 10,
			CreateGameAnswer		= 11,
			JoinGameRequest			= 12,
			JoinGameAnswer			= 13,
			GetGameRequest			= 14,
			GetGameAnswer			= 15,
			LeaveGameRequest		= 16,
			LeaveGameAnswer			= 17,
			DeleteGameRequest		= 18,
			NewPlayer				= 19,
			PlayerReady				= 20,
			PlayerNotReady			= 21,
			StartGame				= 22,
			NewPlayerJoin			= 23,

			// Game Logic
			ObjectEnter				= 50,
			ObjectLeave				= 51,
			ObjectChangeCell		= 52,
			UpdateHealth			= 53,
			UpdateMovementRequest	= 54,
			UpdateMovementAnswer	= 55,

			// /!\ Game view  : 100-150 /!\ 
		};

		Packet(EMessageType messageType);
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

		Packet::EMessageType GetType() const { return m_type; }

		Id GetId() const { return m_uniqueId; }

		uint16_t GetSerializedSize() const { return m_serializedSize; }
		void SetSerializedSize(uint16_t serializedSize) { m_serializedSize = serializedSize; }

		static const uint16_t HEADER_SIZE = 7;
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
		virtual uint16_t EstimateCurrentSerializedSize() const = 0;
		
	private:
		void SetId(Id value) { m_uniqueId = value; }
		Id m_uniqueId;
		Packet::EMessageType m_type = EMessageType::Error;
		uint16_t m_serializedSize = 0x0;

		std::mutex m_packetIdLocker;
	};

#pragma region Deserialization
	template<typename T>
	inline void Packet::DeserializeData(Buffer512& buffer, T& value)
	{
		static_assert(sizeof(T) <= 8, "Size higher than 8 bytes"); // only work for small types

		unsigned char tmp[sizeof(T)];
		buffer.Select(tmp, sizeof(T));

		value = T();

		// we decode the big endian
		for (size_t i = 0; i < sizeof(T); i++)
			value |= tmp[i] << (8 * ((sizeof(T) - 1) - i));

	}

	template<>
	inline void Packet::DeserializeData<ISerializable>(Buffer512& buffer, ISerializable& value)
	{
		value.Deserialize(buffer);
	}

#pragma endregion


#pragma region Serialization
	template<typename T>
	inline void Packet::SerializeData(Buffer512& buffer, const T& value) {
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

	template<>
	inline void Packet::SerializeData<ISerializable>(Buffer512& buffer, const ISerializable& value) {
		value.Serialize(buffer);
	}

#pragma endregion

	using PacketUPtr = std::unique_ptr<Packet>;
	using PacketSPtr = std::shared_ptr<Packet>;
}

