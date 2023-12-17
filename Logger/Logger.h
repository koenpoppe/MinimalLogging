#ifndef LOGGER_H
#define LOGGER_H

#include <chrono>

template <int foo>
class Logger
{
public:
    using Clock = std::chrono::high_resolution_clock;
    using TimeUnit = std::chrono::nanoseconds;
    static inline TimeUnit::rep now()
    {
        return std::chrono::duration_cast<TimeUnit>(Clock::now().time_since_epoch()).count();
    }

private:
    struct Record
    {
        const TimeUnit::rep m_time;
    };
};

#endif // LOGGER_H
