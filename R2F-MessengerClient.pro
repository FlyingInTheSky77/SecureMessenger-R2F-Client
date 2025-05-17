QT += quick core network sql

CONFIG += c++17

# choose one: basic or custom
VARIANT = basic
# VARIANT = custom

message(Build variant: $$VARIANT)

equals(VARIANT, basic) {
    DEFINES += APP_VARIANT_BASIC
    RESOURCES += resourcesBasic.qrc
} else {
    message(added resourcesCustom.qrc)
    RESOURCES += resourcesCustom.qrc
}

SOURCES += \
        src/ClientStuff.cpp \
        src/ConfigLoader.cpp \
        src/SessionKey.cpp \
        src/SqlContactModel.cpp \
        src/SqlConversationModel.cpp \
        src/main.cpp \
        src/BackEnd.cpp

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    include/ClientStuff.h \
    include/ConfigLoader.h \
    include/SessionKey.h \
    include/SqlContactModel.h \
    include/SqlConversationModel.h \
    include/BackEnd.h \
    include/messagecode.h

DISTFILES +=

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../usr/local/lib/release/ -lgmp
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../usr/local/lib/debug/ -lgmp
else:unix: LIBS += -L$$PWD/../../../../../usr/local/lib/ -lgmp

INCLUDEPATH += $$PWD/../../../../../usr/local/lib
DEPENDPATH += $$PWD/../../../../../usr/local/lib

unix:LIBS+= -L/usr/local/lib -lgmp

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../usr/local/lib/release/libgmp.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../usr/local/lib/debug/libgmp.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../usr/local/lib/release/gmp.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../usr/local/lib/debug/gmp.lib
