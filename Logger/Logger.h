#ifndef LOGGER_H
#define LOGGER_H

#include <array>
#include <atomic>
#include <chrono>
#include <cstring>
#include <ostream>

namespace Details
{
template <std::size_t sizeLog2>
class CircularBuffer
{
public:
    static constexpr std::size_t bufferSize = 2 ^ sizeLog2;

    void clear()
    {
        m_nonModTail = 0u;
        m_bufferFull = false;
    }
    template <typename T>
    void append(const T t)
    {
        // NOTE: compiler will replace module power-of-2 by non-branching AND
        const std::size_t tail = m_nonModTail.fetch_add(sizeof(T)) % bufferSize;
        if (tail < bufferSize - sizeof(T))
        {
            // This hot path will typically not create t as such, but prefer a
            // direct copy from the input arguments
            std::memcpy(&m_buffer[tail], &t, sizeof(T));
        }
        else
        {
            m_bufferFull = true;
            const std::size_t firstPart = bufferSize - tail;
            const auto explicitT = t; // only here, the T will be constructed
            std::memcpy(&m_buffer[tail], &explicitT, firstPart);
            std::memcpy(&m_buffer[0], reinterpret_cast<const char *>(&explicitT) + firstPart, sizeof(T) - firstPart);
        }
    }

    void writeTo(std::ostream &s) const
    {
        const std::size_t tail = m_nonModTail % bufferSize;
        if (m_bufferFull)
        {
            s.write(&m_buffer[tail], static_cast<std::streamsize>(bufferSize - tail));
        }
        s.write(m_buffer.data(), static_cast<std::streamsize>(tail));
    }

private:
    std::array<char, bufferSize> m_buffer{};
    std::atomic<std::size_t> m_nonModTail{0u};
    bool m_bufferFull = false;
};

} // namespace Details

template <std::size_t sizeLog2>
class Logger
{
    // Logging
public:
    void trace()
    {
        m_circularBuffer.append(Record{now()});
    }
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

    // Buffer
public:
    void writeTo(std::ostream &s) const
    {
        m_circularBuffer.writeTo(s);
    }

private:
    Details::CircularBuffer<sizeLog2> m_circularBuffer;
};

#endif // LOGGER_H
