/*****************************************************************//**
 * \file   time.hpp
 * \brief  Time Utility Header 
 * 
 * \author sdk
 * \date   June 2023
 *********************************************************************/

#pragma once 

#include <iostream>
#include <cstdint>
#include <ctime>
#include <chrono>
#include <thread>
#include <windows.h>

namespace timeutil {

uint64_t GetUnixTimestamp(int year, int mon, int day, int hour, int min, int sec, int msec = 0);

uint64_t GetUnixTimestampFromSysTime(const SYSTEMTIME& systime);

struct tm GetTMFromEpoch(uint64_t epoch);

uint64_t GetEpochFromTM(struct tm timestamp);

inline uint64_t GetEpochMs() {
	return std::chrono::duration_cast<std::chrono::milliseconds>
		(std::chrono::system_clock::now().time_since_epoch()).count();
}

inline void Sleep(uint64_t msec) {
	std::this_thread::sleep_for(std::chrono::milliseconds(msec));
}

}
