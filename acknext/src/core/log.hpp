#ifndef LOG_HPP
#define LOG_HPP

#include <stdio.h>
#include <chrono>

// This clock point is used for logging
extern std::chrono::steady_clock::time_point startupTime;

extern FILE * logfile;

#endif // LOG_HPP
