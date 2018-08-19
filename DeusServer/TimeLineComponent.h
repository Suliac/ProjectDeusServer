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
#define WANT_DATA_BEFORE_TIMESTAMP true
#define WANT_DATA_AFTER_TIMESTAMP false

	public:
		virtual std::shared_ptr<const T> GetValue(long timeWantedMs = -1) const;
		void InsertData(std::shared_ptr<const T> data);
		void InsertData(std::shared_ptr<const T> data, long ms);
	protected:
		inline std::shared_ptr<const T> GetValueAtTime(long wantedTimeStampMS, long& timeStampOfData, bool wantDataBeforeTimestamp) const;

		virtual std::shared_ptr<T> Interpolate(const T& beforeValue, long beforeValueTimestamp, const T& afterValue, long afterValueTimestamp, long currentMs) const = 0;
		virtual std::shared_ptr<T> Extrapolate(const T& beforeValue, long beforeValueTimestamp, long currentMs) const = 0;

		std::unordered_map<std::shared_ptr<const T>, long> m_dataWithTime;
		const uint32_t MAX_DATAS_SAVED = 10000;

	private:
		mutable std::mutex m_componentLocker;
	};

	template<typename T>
	inline void TimeLineComponent<T>::InsertData(std::shared_ptr<const T> data)
	{
		long ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		InsertData(data, ms);
	}

	template<typename T>
	inline void TimeLineComponent<T>::InsertData(std::shared_ptr<const T> data, long ms)
	{
		m_componentLocker.lock(); // <---------------- LOCK
		m_dataWithTime.insert(std::make_pair(data, ms + DELAY_MS));
		m_componentLocker.unlock(); // <---------------- UNLOCK
	}

	template<typename T>
	inline std::shared_ptr<const T> TimeLineComponent<T>::GetValue(long timeWantedMs) const
	{
		long currentMs = timeWantedMs == -1 ? std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() : timeWantedMs;

		long timeStampBeforeDataFound = -1;
		std::shared_ptr<const T> p_objectInTimeLineBefore = GetValueAtTime(currentMs, timeStampBeforeDataFound, WANT_DATA_BEFORE_TIMESTAMP);

		long timeStampAfterDataFound = -1;
		std::shared_ptr<const T> p_objectInTimeLineAfter = GetValueAtTime(currentMs, timeStampAfterDataFound, WANT_DATA_AFTER_TIMESTAMP);

		std::shared_ptr<const T> p_value = nullptr;

		if (timeStampBeforeDataFound = !- 1 && timeStampAfterDataFound != -1) // we are between 2 value -> interpolate
			p_value = Interpolate(*p_objectInTimeLineBefore, timeStampBeforeDataFound, *p_objectInTimeLineAfter, timeStampAfterDataFound, currentMs);
		else if (timeStampBeforeDataFound = !- 1) // we have value only before the timestamp requested but not after
			p_value = Extrapolate(*p_objectInTimeLineBefore, timeStampBeforeDataFound, currentMs);
		else // no data found or only after the timestamp
			p_value = nullptr;

		return p_value;
	}

	template<typename T>
	inline std::shared_ptr<const T> TimeLineComponent<T>::GetValueAtTime(long wantedTimeStampMS, long & timeStampOfData, bool wantDataBeforeTimestamp) const
	{
		m_componentLocker.lock(); // <---------------- LOCK
		if (m_dataWithTime.size() <= 0)
		{
			m_componentLocker.unlock(); // <---------------- UNLOCK
			return nullptr;
		}
		
		if (wantDataBeforeTimestamp)
		{
			// start from the end
			for (auto it = m_dataWithTime.end(); it != m_dataWithTime.begin(); --it)
			{
				if (it->second < wantedTimeStampMS)
				{
					timeStampOfData = it->second;
					m_componentLocker.unlock(); // <---------------- UNLOCK
					return it->first;
				}
			}
		}
		else {
			// start from the begining
			for (auto it = m_dataWithTime.begin(); it != m_dataWithTime.end(); --it)
			{
				if (it->second > wantedTimeStampMS)
				{
					timeStampOfData = it->second;
					m_componentLocker.unlock(); // <---------------- UNLOCK
					return it->first;
				}
			}
		}

		m_componentLocker.unlock(); // <---------------- UNLOCK
		return nullptr;
	}

	//template<typename T>
	//inline std::shared_ptr<T> TimeLineComponent<T>::GetValueAtTime(long wantedTimeStampMS, long& timeStampOfData, bool wantDataBeforeTimestamp) const
	//{
	//	m_componentLocker.lock(); // <---------------- LOCK
	//	if (m_dataWithTime.size() <= 0)
	//	{
	//		m_componentLocker.unlock(); // <---------------- UNLOCK
	//		return nullptr;
	//	}

	//	typename std::unordered_map<std::shared_ptr<T>, long>::iterator it;

	//	if (wantDataBeforeTimestamp)
	//	{
	//		// start from the end
	//		for (it = m_dataWithTime.end(); it != m_dataWithTime.begin(); --it)
	//		{
	//			if (it->second < wantedTimeStampMS)
	//			{
	//				timeStampOfData = it->second;
	//				m_componentLocker.unlock(); // <---------------- UNLOCK
	//				return it->first;
	//			}
	//		}
	//	}
	//	else {
	//		// start from the begining
	//		for (it = m_dataWithTime.begin(); it != m_dataWithTime.end(); --it)
	//		{
	//			if (it->second > wantedTimeStampMS)
	//			{
	//				timeStampOfData = it->second;
	//				m_componentLocker.unlock(); // <---------------- UNLOCK
	//				return it->first;
	//			}
	//		}
	//	}

	//	m_componentLocker.unlock(); // <---------------- UNLOCK
	//	return nullptr;
	//}	

}

