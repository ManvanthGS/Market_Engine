#pragma once

#include <iomanip>
#include <sstream>
#include <string>

#include "types.hpp"

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
    static TimeStamp Get_Current_Timestamp()
    {
        return std::chrono::high_resolution_clock::now();
    }
};
