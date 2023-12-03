#include <QCoreApplication>
#include <QDebug>
#include <QLoggingCategory>
#include <QTest>
#include <QObject>

class QtLoggingTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();

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

void QtLoggingTest::initTestCase()
{
    qSetMessagePattern("%{time} file: %{file}(%{line}) `%{function}`:%{message}");
        QLoggingCategory::setFilterRules("*.info=false");

    qInfo() << "initTestCase";
}

// Demo

void QtLoggingTest::grandParentFunction()
{
    qDebug() << "grandParent";
}
void QtLoggingTest::parentFunction()
{
    qDebug() << "parent";
    grandParentFunction();
}
void QtLoggingTest::childFunction()
{
    qDebug() << "child";
    parentFunction();
}

void QtLoggingTest::demo()
{
    childFunction();
}

// Example

Q_LOGGING_CATEGORY(app, "app")
void QtLoggingTest::example()
{
    const QString title = "MyApp";
    const QSize windowSize(800, 600);
    qCDebug(app) << "Window title:" << title << "size:" << windowSize;
}

QTEST_GUILESS_MAIN(QtLoggingTest)

#include "01-QtLoggingMain.moc"
