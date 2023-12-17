#ifndef LOGGER_UNIT_TEST_H
#define LOGGER_UNIT_TEST_H

#include <QObject>

class LoggerUnitTest : public QObject
{
    Q_OBJECT

private slots:
    void unsupportedTest();

    void timeInfo();
};

#endif // LOGGER_UNIT_TEST_H
