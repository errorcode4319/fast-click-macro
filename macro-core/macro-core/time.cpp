/*****************************************************************//**
 * \file   time.cpp
 * \brief  Time Utility Source 
 * 
 * \author sdk
 * \date   June 2023
 *********************************************************************/

#include "time.hpp"
#include <ctime>
#include <iostream>


namespace timeutil {

uint64_t GetUnixTimestamp(int year, int mon, int day, int hour, int min, int sec, int msec) {
    struct tm dt;
    dt.tm_year = year - 1900;
    dt.tm_mon = mon - 1;
    dt.tm_mday = day;
    dt.tm_hour = hour;
    dt.tm_min = min;
    dt.tm_sec = sec;
    std::time_t t = mktime(&dt);
    return (uint64_t(t) * 1000) + (msec % 1000);
}

uint64_t GetUnixTimestampFromSysTime(const SYSTEMTIME& systime) {
    auto [year, mon, dayofweek, day, hour, min, sec, msec] = systime;
    std::cout << "Server Time: " << year << ':' << mon << ':' << day << ' ' << hour << ':' << min << ':' << sec << ':' << msec << std::endl;

    return GetUnixTimestamp(
        int(systime.wYear),
        int(systime.wMonth),
        int(systime.wDay),
        int(systime.wHour),
        int(systime.wMinute),
        int(systime.wSecond),
        int(systime.wMilliseconds)
    );
}

struct tm GetTMFromEpoch(uint64_t epoch_ms) {
    struct tm epoch_time;
    time_t seconds = time_t(epoch_ms / 1000);
    memcpy(&epoch_time, localtime(&seconds), sizeof(struct tm));
    return epoch_time;
}

uint64_t GetEpochFromTM(struct tm timestamp) {
    std::time_t t = mktime(&timestamp);
    return (uint64_t(t) * 1000);
}

}
