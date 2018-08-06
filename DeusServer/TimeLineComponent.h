#pragma once
#include <unordered_map>
#include <chrono>
#include <mutex>

#include "GameObjectComponent.h"
namespace DeusServer
{
	
	template<typename T>
	class TimeLineComponent : public GameObjectComponent
	{
	public:
		void InsertData(T data);
		virtual T GetValue() = 0;
	protected:
		void InsertData(T data, long ms);
		const T& GetValueAtTime(long timeStampMS);

		std::unordered_map<T, long> m_dataWithTime;
		const uint32_t MAX_DATAS_SAVED = 10000;
		const uint32_t DELAY_MS = 200;

	private:
		std::mutex m_componentLocker;
	};

	template<typename T>
	inline void TimeLineComponent<T>::InsertData(T data)
	{
		long ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		InsertData(data, ms);
	}

	template<typename T>
	inline void TimeLineComponent<T>::InsertData(T data, long ms)
	{
		m_componentLocker.lock(); // <---------------- LOCK
		m_dataWithTime.insert(std::make_pair(data, ms + DELAY_MS));
		m_componentLocker.unlock(); // <---------------- UNLOCK
	}

	template<typename T>
	inline const T& TimeLineComponent<T>::GetValueAtTime(long wantedTimeStampMS)
	{
		m_componentLocker.lock(); // <---------------- LOCK
		if (m_dataWithTime.size() <= 0)
			return T();
		
		typename std::unordered_map<T, long>::iterator it;
		for (it = m_dataWithTime.end(); it != m_dataWithTime.begin(); --it)
		{
			if (it->second < wantedTimeStampMS)
				return it->first;
		}
		m_componentLocker.unlock(); // <---------------- UNLOCK
	}

}

