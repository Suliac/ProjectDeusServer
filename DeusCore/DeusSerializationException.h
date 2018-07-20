#pragma once
#include "DeusException.h"
namespace DeusCore
{
	class DeusSerializationException : public DeusException
	{
	public:
		DeusSerializationException(std::string message);
		~DeusSerializationException();

	};
}

