#include <iostream>
#include <source_location>
#include <string_view>

#include <QTest>
#include <QObject>

class SourceLocationTest : public QObject
{
    Q_OBJECT

    // Demo
private:
    void grandParentFunction();
    void parentFunction();
    void childFunction();
private slots:
    void demo();

    // Example
private slots:
    void example();
};

template <typename... Ts>
void log(std::string_view message,
         const std::source_location location = std::source_location::current())
{
    std::cout << location.file_name() << "(" << location.line() << ") "
              << location.function_name() << ": " << message << '\n';
}

// Demo

void SourceLocationTest::grandParentFunction()
{
    log("grandParent");
}
void SourceLocationTest::parentFunction()
{
    log("parent");
    grandParentFunction();
}
void SourceLocationTest::childFunction()
{
    log("child");
    parentFunction();
}

void SourceLocationTest::demo()
{
    log("main");
    childFunction();
}

// Examle

void SourceLocationTest::example()
{
    log("Hello");
}

QTEST_GUILESS_MAIN(SourceLocationTest)

#include "03-SourceLocationMain.moc"
