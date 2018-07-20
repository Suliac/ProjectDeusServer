#include "Logger.h"
#include <iomanip>
#include <iostream>

namespace DeusCore
{
	Logger* volatile Logger::p_instance = nullptr;
	std::mutex Logger::m_instanceLock;


	Logger* Logger::Instance()
	{
		if(p_instance == nullptr) {
			m_instanceLock.lock();
			if (p_instance == nullptr) {
				Logger* volatile temp = new Logger; // volatile added
				p_instance = temp;
			}
			m_instanceLock.unlock();
		}

		return p_instance;
	}

	void Logger::Log(std::string name, std::string message)
	{
		m_cliLock.lock();
		std::cout << "[" << std::left <<std::setw(15) << name << "]" << message << std::endl;
		m_cliLock.unlock();
	}

	Logger::Logger()
	{
	}
	
	Logger::~Logger()
	{
		if (p_instance != nullptr)
			delete p_instance;
	}
}
