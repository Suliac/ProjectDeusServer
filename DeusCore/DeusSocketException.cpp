#include "DeusSocketException.h"

namespace DeusNetwork
{
	DeusSocketException::DeusSocketException(std::string message)
	{
		m_message = message;
	}


	DeusSocketException::~DeusSocketException()
	{
	}

	 std::string DeusSocketException::GetErrorMessage() const
	{
		return m_message;
	}
}
