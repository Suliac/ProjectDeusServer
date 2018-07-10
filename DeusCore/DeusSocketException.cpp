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

	std::string DeusSocketException::GetErrorMessage()
	{
		return m_message;
	}
}
