#include "LoggerUnitTest.h"

#include <sstream>

#include <QTest>

// NOTE: LogModel is out of scope and not included in this repository
#include "../Private/LogModel.h"

#include "../Logger/Logger.h"

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

QTEST_APPLESS_MAIN(LoggerUnitTest)
