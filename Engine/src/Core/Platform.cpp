#include "Core/Platform.h"

#include <ctime>
#include <chrono>

namespace Luden
{
	uint64_t Platform::GetCurrentDateTimeU64()
	{
		std::string str = GetCurrentDateTimeString();
		return std::stoull(str);
	}

	std::string Platform::GetCurrentDateTimeString()
	{
		std::time_t currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		std::tm localTime{};
		localtime_s(&localTime, &currentTime);

		int year = localTime.tm_year + 1900;
		int month = localTime.tm_mon + 1;
		int day = localTime.tm_mday;
		int hour = localTime.tm_hour;
		int minute = localTime.tm_min;

		return std::format("{0:2}{0:2}{0:2}{0:2}{0:2}", year, month, day, hour, minute);
	}
}