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
    unsupportedTestImpl(Logger<512u>{});
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
            const LogModel model(std::istringstream{data});
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
    timeInfoImpl<Logger<512u>>();
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
        QCOMPARE(data.size(), sizeof(typename Logger::TimeUnit::rep) + sizeof(uintptr_t));

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
    resolveFunctionSingleImpl<Logger<512u>>();
}

QTEST_APPLESS_MAIN(LoggerUnitTest)
