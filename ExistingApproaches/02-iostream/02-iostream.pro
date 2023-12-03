QT = core testlib

include("../../GlobalSettings.pri")

TESTPROGRAM(02-iostream)

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
