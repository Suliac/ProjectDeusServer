#include "DeusSocketException.h"

namespace DeusNetwork
{
	DeusSocketException::DeusSocketException(std::string message) : DeusException(message)
	{
	}

	DeusSocketException::~DeusSocketException()
	{
	}

}
