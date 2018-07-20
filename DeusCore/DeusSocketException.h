#pragma once
#include "DeusException.h"

namespace DeusCore
{
	class DeusSocketException : public DeusException
	{
	public:
		DeusSocketException(std::string message);
		~DeusSocketException();
	};
}

