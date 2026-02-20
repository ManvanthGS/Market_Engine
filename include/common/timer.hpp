#pragma once

#include <chrono>
#include <iomanip>
#include <sstream>
#include <string>

using TimeStamp = std::chrono::time_point<std::chrono::high_resolution_clock>;

// A static time class to handle time-related operations
class Timer
{
  public:
    // return time stamp in human readable format
    static std::string TimeStamp_To_String(const TimeStamp& timestamp)
    {
        auto time_t = std::chrono::system_clock::to_time_t(
            std::chrono::time_point_cast<std::chrono::seconds>(timestamp));
        std::tm tm = *std::localtime(&time_t);
        std::stringstream ss;
        ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    // Get current timestamp
    static std::string Get_Current_Timestamp()
    {
        return TimeStamp_To_String(std::chrono::high_resolution_clock::now());
    }
};
