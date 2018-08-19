#pragma once
#include "ISerializable.h"
namespace DeusCore
{
	class DeusVector2 : public ISerializable
	{
	public:
		DeusVector2();
		DeusVector2(float x, float y, float precision = 5);

		~DeusVector2();

		float X() const { return m_x / m_precision; }
		float Y() const { return m_y / m_precision; }

		void X(float value) { m_x = value * m_precision; }
		void Y(float value) { m_y = value * m_precision; }

		void XY(float x, float y) {
			m_x = x * m_precision;
			m_y = y * m_precision;
		}

		static DeusVector2 Zero() { return DeusVector2(0, 0); }

		static float Magnitude(const DeusVector2& lhs, const DeusVector2& rhs);

		static float SqrMagnitude(const DeusVector2& lhs, const DeusVector2& rhs);

		bool operator==(const DeusVector2& other) const {
			return m_x == other.m_x &&
				m_y == other.m_y;
		}
		inline bool operator!=(const DeusVector2& other) const {
			return !(*this == other);
		}
		inline DeusVector2 operator+(const DeusVector2& other) const {
			DeusVector2 result;
			result.m_x = result.m_x + other.m_x;
			result.m_y = result.m_y + other.m_y;
			return result;
		}
		inline DeusVector2 operator-(const DeusVector2& other) const {
			DeusVector2 result;
			result.m_x = m_x - other.m_x;
			result.m_y = m_y - other.m_y;
			return result;
		}
		inline DeusVector2 operator*(float scalar) const {
			DeusVector2 result;
			result.m_x = m_x * scalar;
			result.m_y = m_y * scalar;
			return result;
		}

		virtual void Deserialize(Buffer512 & buffer) override;

		virtual void Serialize(Buffer512 & buffer) const override;

		virtual uint16_t EstimateAnswerCurrentSerializedSize() const override;

	private:
		int m_x = 0;
		int m_y = 0;
		float m_precision = 100000.f; // precision 5

	};

}
