#pragma once
#include "ISerializable.h"
namespace DeusCore
{
	class DeusVector2 : public ISerializable
	{
	public:
		DeusVector2();
		DeusVector2(float x, float y, float precision = 8);

		~DeusVector2();

		float X() const { return m_x / m_precision; }
		float Y() const { return m_y / m_precision; }

		float IntX() const { return m_x; }
		float IntY() const { return m_y; }

		void X(float value) { m_x = value * m_precision; }
		void Y(float value) { m_y = value * m_precision; }

		void XY(float x, float y) {
			m_x = x * m_precision;
			m_y = y * m_precision;
		}

		static DeusVector2 Zero() { return DeusVector2(0, 0); }

		inline bool operator==(const DeusVector2& other) {
			return m_x == other.IntX() &&
					m_y == other.IntY();
		}
		inline bool operator!=(const DeusVector2& other) { 
			return m_x != other.IntX() ||
				m_y != other.IntY();
		}

		virtual void Deserialize(Buffer512 & buffer) override;

		virtual void Serialize(Buffer512 & buffer) const override;

		virtual uint16_t EstimateAnswerCurrentSerializedSize() const override;

	private:
		int m_x = 0;
		int m_y = 0;
		float m_precision = 100000000.f; // precision 8

	};

}
