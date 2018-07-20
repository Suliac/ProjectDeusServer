#pragma once
#include <exception>
#include <string>
namespace DeusCore
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

