CONFIG -= app_bundle
CONFIG += c++2b
CONFIG += console
CONFIG += warn_on
CONFIG += silent

# Always include symbols
QMAKE_CXXFLAGS += -g

DESTDIR = $${OUT_PWD}
OBJECTS_DIR = $${DESTDIR}

TARGETFILEPATH = $${DESTDIR}/$${TARGET}

defineTest(TESTPROGRAM) {
    NAME = $$1
    SOURCES += $${NAME}Main.cpp
    export(SOURCES)
}
