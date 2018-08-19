#include "DeusVector2.h"

#include <math.h>

namespace DeusCore
{
	DeusVector2::DeusVector2()
	{
	}

	DeusVector2::DeusVector2(float x, float y, float precision)
	{
		m_precision = pow(10, precision);
		m_x = x * m_precision;
		m_y = y * m_precision;
	}

	DeusVector2::~DeusVector2()
	{
	}

	float DeusVector2::Magnitude(const DeusVector2& lhs, const DeusVector2& rhs)
	{
		return sqrt(SqrMagnitude(lhs, rhs));
	}

	float DeusVector2::SqrMagnitude(const DeusVector2& lhs, const DeusVector2& rhs)
	{
		float precision = fmin(lhs.m_precision, rhs.m_precision);
		return pow((rhs.m_x - lhs.m_x) / precision, 2) +
			pow((rhs.m_y - lhs.m_y) / precision, 2);
	}

	void DeusVector2::Deserialize(Buffer512 & buffer)
	{
		DeserializeData(buffer, m_x);
		DeserializeData(buffer, m_y);

		long tmpPrecision = 0;
		DeserializeData(buffer, tmpPrecision);
		m_precision = tmpPrecision;
	}
	void DeusVector2::Serialize(Buffer512 & buffer) const
	{
		SerializeData(buffer, m_x);
		SerializeData(buffer, m_y);
		SerializeData(buffer, (long)m_precision);
	}

	uint16_t DeusVector2::EstimateAnswerCurrentSerializedSize() const
	{
		return sizeof(m_x) + sizeof(m_y) + sizeof(m_precision);
	}
}
