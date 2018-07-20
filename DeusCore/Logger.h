#pragma once
#include <mutex>
#include <string>
namespace DeusCore
{
	class Logger
	{
	public:
		static Logger* Instance();

		void Log(std::string name, std::string message);
	private:

		static Logger* p_instance;
		static std::mutex m_instanceLock;

		std::mutex m_cliLock;

		Logger();
		~Logger();
	};
}

