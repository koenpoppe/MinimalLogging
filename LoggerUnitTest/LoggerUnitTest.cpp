#include "LoggerUnitTest.h"

#include <sstream>

#include <QTest>

// NOTE: LogModel is out of scope and not included in this repository
#include "../Private/LogModel.h"

#include "../Logger/Logger.h"

namespace QTest
{
bool qCompare(const std::string &t1, const char *t2, const char *actual, const char *expected, const char *file,
              int line)
{
    return qCompare(QString::fromStdString(t1), QString::fromStdString(t2), actual, expected, file, line);
}

#define QCONTAINS(actual, expected) \
    QVERIFY2(actual.find(expected) != std::string::npos, (actual + " must contain " + expected).c_str())

} // namespace QTest

const std::string LoggerUnitTest::s_symbolFilePath{"LoggerUnitTest.syms"};

namespace {
template <typename Logger>
std::string serialize(const Logger &logger)
{
    std::ostringstream stream;
    logger.writeTo(stream);
    return stream.str();
}
}

void unsupportedTestImpl(auto logger)
{
    if constexpr (requires { logger.rocketscience(); })
    {
        logger.rocketscience();
        QFAIL("This is not implemented yet");
    }
    else
    {
        QVERIFY(true); // Branch should be taken
    }
}
void LoggerUnitTest::unsupportedTest()
{
    unsupportedTestImpl(Logger<0u>{});
}

template <typename Logger>
void timeInfoImpl()
{
    if constexpr (requires(Logger l) {
                      l.trace();
                      Logger::now();
                  })
    {
        // Test program
        Logger logger;
        logger.trace();
        QTest::qWait(1000);
        logger.trace();

        // Serialize
        const std::string data = serialize(logger);

        // Check output
        try
        {
            const LogModel model(std::istringstream{data}, LoggerUnitTest::s_symbolFilePath);
            const std::vector<LogModel::Record> &records = model.records();
            QCOMPARE(records.size(), 2u);

            const LogModel::Record before = records.at(0);
            const LogModel::Record after = records.at(1);
            qDebug() << "Duration:" << after.time - before.time;
        }
        catch (const std::exception &e)
        {
            QFAIL(e.what());
        }
    }
    else
    {
        QFAIL("Does not compile");
    }
}
void LoggerUnitTest::timeInfo()
{
    timeInfoImpl<Logger<6u>>();
}

template <typename Logger>
void resolveFunctionSingleImpl()
{
    if constexpr (requires(Logger l) { l.trace(); })
    {
        // Test program
        Logger logger;
        logger.trace();

        // Serialize
        const std::string data = serialize(logger);
        QCOMPARE(data.size(), sizeof(typename Logger::TimeUnit::rep) + sizeof(uintptr_t) + sizeof(uintptr_t));

        // Check output
        try
        {
            const LogModel model(std::istringstream{data}, LoggerUnitTest::s_symbolFilePath);
            const std::vector<LogModel::Record> &records = model.records();
            QCOMPARE(records.size(), 1u);
            QCONTAINS(model.resolveFunction(records.at(0).address), "resolveFunctionSingleImpl");
        }
        catch (const std::exception &e)
        {
            QFAIL(e.what());
        }
    }
    else
    {
        QFAIL("Does not compile");
    }
}
void LoggerUnitTest::resolveFunctionSingle()
{
    resolveFunctionSingleImpl<Logger<5u>>();
}

template <typename L>
struct ResolveFunctionTestClass
{
    L logger;
    void first() __attribute__((noinline))
    {
        logger.trace();
        second();
        logger.trace();
    }
    void second() __attribute__((noinline))
    {
        logger.trace();
    }
};
template <typename Logger>
void resolveFunctionsNestedImpl()
{
    if constexpr (requires(Logger l) { l.trace(); })
    {
        // Test program
        ResolveFunctionTestClass<Logger> test;
        test.first();

        // Serialize
        const std::string data = serialize(test.logger);
        QCOMPARE(data.size(), 3*(sizeof(typename Logger::TimeUnit::rep) + sizeof(uintptr_t) + sizeof(uintptr_t)));

        // Check output
        try
        {
            const LogModel model(std::istringstream{data}, LoggerUnitTest::s_symbolFilePath);
            const std::vector<LogModel::Record> &records = model.records();
            QCOMPARE(records.size(), 3u);
            QCONTAINS(model.resolveFunction(records.at(0).address), "::first");
            QCONTAINS(model.resolveFunction(records.at(1).address), "::second");
            QCONTAINS(model.resolveFunction(records.at(2).address), "::first");
        }
        catch (const std::exception &e)
        {
            QFAIL(e.what());
        }
    }
    else
    {
        QFAIL("Does not compile");
    }
}
void LoggerUnitTest::resolveFunctionsNested()
{
    resolveFunctionsNestedImpl<Logger<7u>>();
}

template <typename Logger>
void traceWithBoolImpl()
{
    if constexpr (requires(Logger l) { l.template trace<bool>({}); })
    {
        // Test
        Logger logger;
        logger.trace(true);
        logger.trace(false);

        // Serialize
        const std::string data = serialize(logger);

        // Check output
        try
        {
            const LogModel model(std::istringstream{data}, LoggerUnitTest::s_symbolFilePath);
            const std::vector<LogModel::Record> &records = model.records();
            QCOMPARE(records.size(), 2u);
            {
                LogModel::Record trueRecord = records.at(0);
                QCOMPARE(trueRecord.args.size(), 1u);
                QCOMPARE(std::any_cast<bool>(trueRecord.args.front()), true);
            }
            {
                LogModel::Record falseRecord = records.at(1);
                QCOMPARE(falseRecord.args.size(), 1u);
                QCOMPARE(std::any_cast<bool>(falseRecord.args.front()), false);
            }
        }
        catch (const std::exception &e)
        {
            QFAIL(e.what());
        }
    }
    else
    {
        QFAIL("Does not compile");
    }
}
void LoggerUnitTest::traceWithBool()
{
    traceWithBoolImpl<Logger<6u>>();
}

template <typename Logger>
void traceWithIntImpl()
{
    if constexpr (requires(Logger l) { l.template trace<std::int64_t>({}); })
    {
        // Test
        Logger logger;
        logger.trace(std::int8_t{-88});
        logger.trace(std::int16_t{-16161});
        logger.trace(std::int32_t{-323232323});
        logger.trace(std::int64_t{-6464646464646464646});

        // Serialize
        const std::string data = serialize(logger);

        // Check output
        try
        {
            const LogModel model(std::istringstream{data}, LoggerUnitTest::s_symbolFilePath);
            const std::vector<LogModel::Record> &records = model.records();
            QCOMPARE(records.size(), 4u);

            QCOMPARE(records.at(0).args.size(), 1u);
            QCOMPARE(std::any_cast<std::int8_t>(records.at(0).args.front()), int8_t{-88});
            QCOMPARE(records.at(1).args.size(), 1u);
            QCOMPARE(std::any_cast<std::int16_t>(records.at(1).args.front()), int16_t{-16161});
            QCOMPARE(records.at(2).args.size(), 1u);
            QCOMPARE(std::any_cast<std::int32_t>(records.at(2).args.front()), int32_t{-323232323});
            QCOMPARE(records.at(3).args.size(), 1u);
            QCOMPARE(std::any_cast<std::int64_t>(records.at(3).args.front()), int64_t{-6464646464646464646});
        }
        catch (const std::exception &e)
        {
            QFAIL(e.what());
        }
    }
    else
    {
        QFAIL("Does not compile");
    }
}
void LoggerUnitTest::traceWithInt()
{
    traceWithIntImpl<Logger<7u>>();
}

template <typename Logger>
void traceWithUnsignedIntImpl()
{
    if constexpr (requires(Logger l) { l.template trace<std::uint64_t>({}); })
    {
        // Test
        Logger logger;
        logger.trace(std::uint8_t{88});
        logger.trace(std::uint16_t{16161});
        logger.trace(std::uint32_t{3232323232});
        logger.trace(std::uint64_t{6464646464646464646});
        logger.trace(std::size_t{1234567891011121314});

        // Serialize
        const std::string data = serialize(logger);

        // Check output
        try
        {
            const LogModel model(std::istringstream{data}, LoggerUnitTest::s_symbolFilePath);
            const std::vector<LogModel::Record> &records = model.records();
            QCOMPARE(records.size(), 5u);

            QCOMPARE(records.at(0).args.size(), 1u);
            QCOMPARE(std::any_cast<std::uint8_t>(records.at(0).args.front()), uint8_t{88});
            QCOMPARE(records.at(1).args.size(), 1u);
            QCOMPARE(std::any_cast<std::uint16_t>(records.at(1).args.front()), uint16_t{16161});
            QCOMPARE(records.at(2).args.size(), 1u);
            QCOMPARE(std::any_cast<std::uint32_t>(records.at(2).args.front()), uint32_t{3232323232});
            QCOMPARE(records.at(3).args.size(), 1u);
            QCOMPARE(std::any_cast<std::uint64_t>(records.at(3).args.front()), uint64_t{6464646464646464646});
            QCOMPARE(records.at(4).args.size(), 1u);
            QCOMPARE(std::any_cast<std::size_t>(records.at(4).args.front()), uint64_t{1234567891011121314});
        }
        catch (const std::exception &e)
        {
            QFAIL(e.what());
        }
    }
    else
    {
        QFAIL("Does not compile");
    }
}
void LoggerUnitTest::traceWithUnsignedInt()
{
    traceWithUnsignedIntImpl<Logger<8u>>();
}

template <typename Logger>
void traceWithFloatImpl()
{
    if constexpr (requires(Logger l) { l.template trace<long double>({}); })
    {
        // Test
        Logger logger;
        logger.trace(3.14159f);
        logger.trace(1.618033988749895);
        logger.trace(0.3333333333333333333333333333333333L);

        // Serialize
        const std::string data = serialize(logger);

        // Check output
        try
        {
            const LogModel model(std::istringstream{data}, LoggerUnitTest::s_symbolFilePath);
            const std::vector<LogModel::Record> &records = model.records();
            QCOMPARE(records.size(), 3u);

            QCOMPARE(records.at(0).args.size(), 1u);
            QCOMPARE(std::any_cast<float>(records.at(0).args.front()), 3.14159f);
            QCOMPARE(records.at(1).args.size(), 1u);
            QCOMPARE(std::any_cast<double>(records.at(1).args.front()), 1.618033988749895);
            QCOMPARE(records.at(2).args.size(), 1u);
            QCOMPARE(std::any_cast<long double>(records.at(2).args.front()), 0.3333333333333333333333333333333333L);
        }
        catch (const std::exception &e)
        {
            QFAIL(e.what());
        }
    }
    else
    {
        QFAIL("Does not compile");
    }
}
void LoggerUnitTest::traceWithFloat()
{
    traceWithFloatImpl<Logger<7u>>();
}

template <typename Logger>
void traceMultiImpl()
{
    if constexpr (requires(Logger l) { l.template trace<bool, int, float>({}, {}, {}); })
    {
        // Test
        Logger logger;
        logger.trace(true, 42, 3.14159f);

        // Serialize
        const std::string data = serialize(logger);

        // Check output
        try
        {
            const LogModel model(std::istringstream{data}, LoggerUnitTest::s_symbolFilePath);
            const std::vector<LogModel::Record> &records = model.records();
            QCOMPARE(records.size(), 1u);

            const LogModel::Record first = records.at(0);
            QCOMPARE(first.args.size(), 3u);
            QCOMPARE(std::any_cast<bool>(first.args.at(0)), true);
            QCOMPARE(std::any_cast<int>(first.args.at(1)), 42);
            QCOMPARE(std::any_cast<float>(first.args.at(2)), 3.14159f);
        }
        catch (const std::exception &e)
        {
            QFAIL(e.what());
        }
    }
    else
    {
        QFAIL("Does not compile");
    }
}
void LoggerUnitTest::traceMulti()
{
    traceMultiImpl<Logger<6u>>();
}

QTEST_APPLESS_MAIN(LoggerUnitTest)
