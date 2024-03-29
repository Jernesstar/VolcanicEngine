#pragma once

#include <chrono>

namespace VolcaniCore {

using Clock = std::chrono::high_resolution_clock;
using Time_Point = std::chrono::time_point<Clock>;
using Duration = std::chrono::duration<float, std::milli>;

class TimePoint {
public:
	TimePoint() = default;
	TimePoint(const Time_Point& time_point) : m_TimePoint(time_point) { }
	~TimePoint() = default;

	float operator -(const TimePoint& other)
	{
		Duration duration = this->m_TimePoint - other.m_TimePoint;
		return (float)duration.count();
	}

private:
	Time_Point m_TimePoint;
};

class TimeStep {
public:
	TimeStep(float time = 0.0f) : m_Time(time) { }

	operator float() const { return m_Time; }

private:
	float m_Time;
};

class Time {
public:
	static TimePoint GetTime() { return { Clock::now() }; }
};
}