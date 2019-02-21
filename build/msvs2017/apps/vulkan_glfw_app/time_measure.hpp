#pragma once
#include <chrono>

// structs

typedef struct TimeStamp {
	std::chrono::time_point<std::chrono::steady_clock> prevTimePoint;
	std::chrono::time_point<std::chrono::steady_clock> nextTimePoint;
	float                                              accumTime;
	float                                              deltaTime;
	float                                              printTime;
	uint32_t                                           ticks;
} TimeStamp;

// functions

void timeStampTick(TimeStamp& timeStamp);
void timeStampReset(TimeStamp& timeStamp);
void timeStampPrint(TimeStamp& timeStamp, float period);
