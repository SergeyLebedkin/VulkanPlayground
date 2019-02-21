#include "time_measure.hpp"
#include <iostream>

// timeStampTick
void timeStampTick(TimeStamp& timeStamp) 
{
	timeStamp.prevTimePoint = timeStamp.nextTimePoint;
	timeStamp.nextTimePoint = std::chrono::high_resolution_clock::now();
	timeStamp.deltaTime = std::chrono::duration_cast<std::chrono::duration<float>>(timeStamp.nextTimePoint - timeStamp.prevTimePoint).count();
	timeStamp.printTime = timeStamp.printTime + timeStamp.deltaTime;
	timeStamp.accumTime = timeStamp.accumTime + timeStamp.deltaTime;
	timeStamp.ticks++;
};

// timeStampReset
void timeStampReset(TimeStamp& timeStamp)
{
	timeStamp.prevTimePoint = std::chrono::high_resolution_clock::now();
	timeStamp.nextTimePoint = std::chrono::high_resolution_clock::now();
	timeStamp.deltaTime = 0.0f;
	timeStamp.printTime = 0.0f;
	timeStamp.accumTime = 0.0f;
	timeStamp.ticks = 0;
};

// timeStampPrint
void timeStampPrint(std::ostream& os, TimeStamp& timeStamp, float period)
{
	if (timeStamp.printTime >= period) {
		os << timeStamp.ticks << " ticks in " << timeStamp.printTime << std::endl;
		timeStamp.printTime = 0.0f;
		timeStamp.ticks = 0;
	}
};
