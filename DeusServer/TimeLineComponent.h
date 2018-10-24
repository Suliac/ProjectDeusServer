#pragma once
#include <map>
#include <chrono>
#include <mutex>

#include "GameObjectComponent.h"
#include "DeusCore/Logger.h"
namespace DeusServer
{
	template<typename T>
	class TimeLineComponent : public GameObjectComponent
	{
#define WANT_DATA_BEFORE_TIMESTAMP true
#define WANT_DATA_AFTER_TIMESTAMP false

	public:
		TimeLineComponent(EComponentType type, Id parentObjectId);

		virtual std::shared_ptr<const T> GetValue(uint32_t timeWantedMs = 0) const;
		void InsertData(std::shared_ptr<const T> data);
		void InsertData(std::shared_ptr<const T> data, uint32_t ms);
		inline std::shared_ptr<const T> GetValueAtTime(uint32_t wantedTimeStampMS, uint32_t& timeStampOfData, bool wantDataBeforeTimestamp) const;
	protected:

		virtual std::shared_ptr<T> Interpolate(const T& beforeValue, uint32_t beforeValueTimestamp, const T& afterValue, uint32_t afterValueTimestamp, uint32_t currentMs) const = 0;
		virtual std::shared_ptr<T> Extrapolate(const T& beforeValue, uint32_t beforeValueTimestamp, uint32_t currentMs) const = 0;

		std::map<uint32_t, std::shared_ptr<const T>> m_dataWithTime;
		const uint32_t MAX_DATAS_SAVED = 10000;

	private:
		mutable std::mutex m_componentLocker;
	};

	template<typename T>
	inline void TimeLineComponent<T>::InsertData(std::shared_ptr<const T> data)
	{
		uint32_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
		InsertData(data, ms);
	}

	template<typename T>
	inline void TimeLineComponent<T>::InsertData(std::shared_ptr<const T> data, uint32_t ms)
	{
		m_componentLocker.lock(); // <---------------- LOCK
		// We delete all the futur datas that arn't valid anymore
		auto it = m_dataWithTime.begin();
		while (it != m_dataWithTime.end()) {
			if (it->first > ms)
			{
				it = m_dataWithTime.erase(it);
			}
			else
				++it;
		}

		// insert data
		m_dataWithTime.insert(std::make_pair(ms, data));
		m_componentLocker.unlock(); // <---------------- UNLOCK
	}

	template<typename T>
	inline TimeLineComponent<T>::TimeLineComponent(EComponentType type, Id parentObjectId)
		: GameObjectComponent(type, parentObjectId)
	{
	}

	template<typename T>
	inline std::shared_ptr<const T> TimeLineComponent<T>::GetValue(uint32_t timeWantedMs) const
	{
		uint32_t currentMs = timeWantedMs == 0 ? std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() : timeWantedMs;

		uint32_t timeStampBeforeDataFound = 0;
		std::shared_ptr<const T> p_objectInTimeLineBefore = GetValueAtTime(currentMs, timeStampBeforeDataFound, WANT_DATA_BEFORE_TIMESTAMP);

		uint32_t timeStampAfterDataFound = 0;
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
	inline std::shared_ptr<const T> TimeLineComponent<T>::GetValueAtTime(uint32_t wantedTimeStampMS, uint32_t & timeStampOfData, bool wantDataBeforeTimestamp) const
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
			for (auto it = m_dataWithTime.rbegin(); it != m_dataWithTime.rend(); ++it)
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
			for (auto it = m_dataWithTime.begin(); it != m_dataWithTime.end(); ++it)
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

