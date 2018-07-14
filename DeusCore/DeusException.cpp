#include "DeusException.h"



namespace DeusNetwork
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