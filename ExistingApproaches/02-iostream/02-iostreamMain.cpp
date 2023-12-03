#include <iostream>
#include <numbers>

#include <QTest>
#include <QObject>

class iostreamTest : public QObject
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

#define LOG(message)                                                  \
    std::cout << __FILE__ << "(" << __LINE__ << ") " << __FUNCTION__ \
              << ": " << message << "\n"

// Demo

void iostreamTest::grandParentFunction()
{
    LOG("grandParent");
}
void iostreamTest::parentFunction()
{
    LOG("parent");
    grandParentFunction();
}
void iostreamTest::childFunction()
{
    LOG("child");
    parentFunction();
}
void iostreamTest::demo()
{
    LOG("main");
    childFunction();
}

void iostreamTest::example()
{
    LOG("Hello " << std::setprecision(3) << std::numbers::pi);
}

QTEST_GUILESS_MAIN(iostreamTest)

#include "02-iostreamMain.moc"
