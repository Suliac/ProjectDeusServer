#include "DeusException.h"



namespace DeusCore
{
	DeusException::DeusException(std::string message)
	{
		m_message = message;
	}


	DeusException::~DeusException()
	{
	}

	std::string DeusException::GetErrorMessage() const
	{
		return m_message;
	}
}