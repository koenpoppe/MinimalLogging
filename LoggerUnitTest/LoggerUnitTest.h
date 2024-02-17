#ifndef LOGGER_UNIT_TEST_H
#define LOGGER_UNIT_TEST_H

#include <string>

#include <QObject>

class LoggerUnitTest : public QObject
{
    Q_OBJECT

private slots:
    void unsupportedTest();

    void timeInfo();

    void resolveFunctionSingle();
    void resolveFunctionsNested();

    void traceTriviallyCopyable();

    void traceWithBool();
    void traceWithInt();
    void traceWithUnsignedInt();
    void traceWithFloat();

    void traceMulti();

public:
    static const std::string s_symbolFilePath;
};

#endif // LOGGER_UNIT_TEST_H
