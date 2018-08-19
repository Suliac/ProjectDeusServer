#pragma once
#include <map>
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
		virtual std::shared_ptr<const T> GetValue(unsigned long timeWantedMs = 0) const;
		void InsertData(std::shared_ptr<const T> data);
		void InsertData(std::shared_ptr<const T> data, unsigned long ms);
	protected:
		inline std::shared_ptr<const T> GetValueAtTime(unsigned long wantedTimeStampMS, unsigned long& timeStampOfData, bool wantDataBeforeTimestamp) const;

		virtual std::shared_ptr<T> Interpolate(const T& beforeValue, unsigned long beforeValueTimestamp, const T& afterValue, unsigned long afterValueTimestamp, unsigned long currentMs) const = 0;
		virtual std::shared_ptr<T> Extrapolate(const T& beforeValue, unsigned long beforeValueTimestamp, unsigned long currentMs) const = 0;

		std::map<unsigned long, std::shared_ptr<const T>> m_dataWithTime;
		const uint32_t MAX_DATAS_SAVED = 10000;

	private:
		mutable std::mutex m_componentLocker;
	};

	template<typename T>
	inline void TimeLineComponent<T>::InsertData(std::shared_ptr<const T> data)
	{
		unsigned long ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		InsertData(data, ms);
	}

	template<typename T>
	inline void TimeLineComponent<T>::InsertData(std::shared_ptr<const T> data, unsigned long ms)
	{
		m_componentLocker.lock(); // <---------------- LOCK
		// We delete all the futur datas that arn't valid anymore
		auto it = m_dataWithTime.begin();
		while (it != m_dataWithTime.end()) {
			if (it->first > ms + DELAY_MS)
				it = m_dataWithTime.erase(it);
			else
				++it;
		}

		// insert data
		m_dataWithTime.insert(std::make_pair(ms + DELAY_MS, data));
		m_componentLocker.unlock(); // <---------------- UNLOCK
	}

	template<typename T>
	inline std::shared_ptr<const T> TimeLineComponent<T>::GetValue(unsigned long timeWantedMs) const
	{
		unsigned long currentMs = timeWantedMs == 0 ? std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() : timeWantedMs;

		unsigned long timeStampBeforeDataFound = 0;
		std::shared_ptr<const T> p_objectInTimeLineBefore = GetValueAtTime(currentMs, timeStampBeforeDataFound, WANT_DATA_BEFORE_TIMESTAMP);

		unsigned long timeStampAfterDataFound = 0;
		std::shared_ptr<const T> p_objectInTimeLineAfter = GetValueAtTime(currentMs, timeStampAfterDataFound, WANT_DATA_AFTER_TIMESTAMP);

		std::shared_ptr<const T> p_value = nullptr;

		if (timeStampBeforeDataFound > 0 && timeStampAfterDataFound > 0) // we are between 2 value -> interpolate
			p_value = Interpolate(*p_objectInTimeLineBefore, timeStampBeforeDataFound, *p_objectInTimeLineAfter, timeStampAfterDataFound, currentMs);
		else if (timeStampBeforeDataFound > 0) // we have value only before the timestamp requested but not after
			p_value = Extrapolate(*p_objectInTimeLineBefore, timeStampBeforeDataFound, currentMs);
		else // no data found or only after the timestamp
			p_value = nullptr;

		return p_value;
	}

	template<typename T>
	inline std::shared_ptr<const T> TimeLineComponent<T>::GetValueAtTime(unsigned long wantedTimeStampMS, unsigned long & timeStampOfData, bool wantDataBeforeTimestamp) const
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
				if (it->first < wantedTimeStampMS)
				{
					timeStampOfData = it->first;
					m_componentLocker.unlock(); // <---------------- UNLOCK
					return it->second;
				}
			}
		}
		else {
			// start from the begining
			for (auto it = m_dataWithTime.begin(); it != m_dataWithTime.end(); --it)
			{
				if (it->first > wantedTimeStampMS)
				{
					timeStampOfData = it->first;
					m_componentLocker.unlock(); // <---------------- UNLOCK
					return it->second;
				}
			}
		}

		m_componentLocker.unlock(); // <---------------- UNLOCK
		return nullptr;
	}

}

