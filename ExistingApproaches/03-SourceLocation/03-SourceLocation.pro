QT = core testlib

include("../../GlobalSettings.pri")

TESTPROGRAM(03-SourceLocation)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
