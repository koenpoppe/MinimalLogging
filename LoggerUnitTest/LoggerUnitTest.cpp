#include "LoggerUnitTest.h"

#include <QTest>

#include "../Logger/Logger.h"

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

QTEST_APPLESS_MAIN(LoggerUnitTest)
