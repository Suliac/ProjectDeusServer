#pragma once
#include "DeusException.h"
namespace DeusNetwork
{
	class DeusSerializationException : public DeusException
	{
	public:
		DeusSerializationException(std::string message);
		~DeusSerializationException();

	};
}

