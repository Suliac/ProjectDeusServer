#pragma once
#include <queue>
#include <mutex>

namespace DeusCore
{
	// from https://www.justsoftwaresolutions.co.uk/threading/implementing-a-thread-safe-queue-using-condition-variables.html
	// Written by Anthony Williams
	template<typename T>
	class concurrent_queue
	{
	private:
		std::queue<T> m_queue;
		mutable std::mutex m_mutex;
		std::condition_variable m_condition_variable;
	public:
		void push(T const& data)
		{
			std::unique_lock<std::mutex> lock(m_mutex);

			m_queue.push(data);

			lock.unlock();
			m_condition_variable.notify_one();
		}

		bool empty() const
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			return m_queue.empty();
		}

		bool try_pop(T& popped_value)
		{
			std::unique_lock<std::mutex> lock(m_mutex);

			if (m_queue.empty())
				return false;

			popped_value = m_queue.front();
			m_queue.pop();
			return true;
		}

		void wait_and_pop(T& popped_value)
		{
			std::unique_lock<std::mutex> lock(m_mutex);
			while (m_queue.empty())
				m_condition_variable.wait(lock);

			popped_value = m_queue.front();
			m_queue.pop();
		}

	};

}

