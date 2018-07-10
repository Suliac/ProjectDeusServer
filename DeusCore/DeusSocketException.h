#pragma once
#include <exception>
#include <iostream>

namespace DeusNetwork
{
	class DeusSocketException : public std::exception
	{
	public:
		DeusSocketException(std::string message);
		~DeusSocketException();

		std::string GetErrorMessage();

	private:
		std::string m_message;
	};
}

