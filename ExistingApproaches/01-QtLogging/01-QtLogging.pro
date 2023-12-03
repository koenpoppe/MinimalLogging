QT = core testlib

include("../../GlobalSettings.pri")

TESTPROGRAM(01-QtLogging)

# Add logging context
DEFINES += QT_MESSAGELOGCONTEXT
