QT = core testlib

include("../GlobalSettings.pri")

HEADERS += LoggerUnitTest.h
SOURCES += LoggerUnitTest.cpp

DEFINES += ARM
HEADERS += ../Logger/Logger.h

include("../Private/Private.pri")
