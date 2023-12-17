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
    template <typename... Ts>
    void trace(const Ts... args) __attribute__((always_inline))
    {
        traceInner(instructionPointer(), args...);
    }
    template <typename... Ts>
    inline void __attribute__((noinline, used)) traceInner(const uintptr_t traceCallSite, const Ts... args)
    {
        m_circularBuffer.append(RecordT<Ts...>{now(), traceCallSite, instructionPointer(), args...});
    }

    static inline uintptr_t instructionPointer() __attribute__((always_inline))
    {
        uintptr_t ip;
#ifdef ARM
        asm volatile("ADR %0, ." : "=r"(ip));
#elifdef X86
        asm volatile("lea (%%rip),%0" : "=r"(ip));
#else
        static_assert(false, "No implementation to retrieve the instruction pointer");
#endif
        return ip;
    }

    using Clock = std::chrono::high_resolution_clock;
    using TimeUnit = std::chrono::nanoseconds;
    static inline TimeUnit::rep now()
    {
        return std::chrono::duration_cast<TimeUnit>(Clock::now().time_since_epoch()).count();
    }

private:
    struct __attribute__((packed)) Record
    {
        const TimeUnit::rep m_time;
        const uintptr_t m_traceCallSite; /// where is trace called from?
        const uintptr_t m_traceInnerInstance; /// what templated form?
    };

    template <std::size_t I, typename T>
    struct __attribute__((packed)) RecordArg
    {
        T arg;
    };

    template <typename...>
    struct __attribute__((packed)) RecordT;
    template <std::size_t... Is, typename... Ts>
    struct __attribute__((packed)) RecordT<std::index_sequence<Is...>, Ts...> : Record, RecordArg<Is, Ts>...
    {
        RecordT(const TimeUnit::rep time, uintptr_t callerLocation, uintptr_t traceLocation, const Ts... args)
            : Record{time, callerLocation, traceLocation}, RecordArg<Is, Ts>{args}...
        {
        }
    };
    template <typename... Ts>
    struct RecordT : RecordT<std::make_index_sequence<sizeof...(Ts)>, Ts...>
    {
        using RecordT<std::make_index_sequence<sizeof...(Ts)>, Ts...>::RecordT;
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
