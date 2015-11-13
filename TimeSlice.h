#pragma once

#include <thread>
#include <future>

template <typename T>
class TimeSlice
{
private:
	std::thread toRun;
	std::future<T> returnValue;
	bool isRunning;

public:
	TimeSlice();
	template<class FN, class... ARGS> TimeSlice(FN&&, ARGS&&... ax);

	inline bool running();
	inline T getValue();
};

template <typename T>
TimeSlice<T>::TimeSlice()
{
	toRun = std::thread();
	returnValue = std::future<T>();
	isRunning = false;
}

template <typename T>
template<class FN, class... ARGS>
TimeSlice<T>::TimeSlice(FN&& fn, ARGS&&... ax)
{
	returnValue = std::async(fn, ax...);
	isRunning = true;
}

template <typename T>
bool TimeSlice<T>::running()
{
	if (returnValue.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
	{
		isRunning = false;
	}
	else
	{
		isRunning = true;
	}

	return isRunning;
};

template <typename T>
T TimeSlice<T>::getValue()
{
	if (!isRunning)
	{
		return returnValue.get();
	}
	return T();
}