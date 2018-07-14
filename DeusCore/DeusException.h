#pragma once
#include <exception>
#include <string>
namespace DeusNetwork
{
	class DeusException : public std::exception
	{
	public:
		DeusException(std::string message);
		~DeusException();
		std::string GetErrorMessage() const;

	protected:
		std::string m_message;
	};
}

