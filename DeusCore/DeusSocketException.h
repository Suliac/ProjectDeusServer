#pragma once
#include "DeusException.h"

namespace DeusNetwork
{
	class DeusSocketException : public DeusException
	{
	public:
		DeusSocketException(std::string message);
		~DeusSocketException();
	};
}

