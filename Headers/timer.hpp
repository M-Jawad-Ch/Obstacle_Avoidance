#pragma once

#include<chrono>

class Timer
{
    std::chrono::high_resolution_clock::time_point start, end;
    bool recorded = false;
public:

    Timer()
    {
        start = std::chrono::high_resolution_clock::now();
    }

    std::chrono::duration<double> getInterval()
    {
        return std::chrono::high_resolution_clock::now() - start;
    }

    std::chrono::duration<double> getRecordedInterval()
    {
        if (recorded) return end - start;
        else return std::chrono::high_resolution_clock::now() - start;
    }

    void reset()
    {
        start = std::chrono::high_resolution_clock::now();
    }

    void stop()
    {
        end = std::chrono::high_resolution_clock::now();
        recorded = true;
    }
};