QT       += qml quick #core gui declarative

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    cryptocontroller.cpp \
    main.cpp

HEADERS += \
    cryptocontroller.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    main.qml

RESOURCES += \
    resourses.qrc

win32 {
LIBS += F:\Qt\Tools\OpenSSL\Win_x64\lib\libssl.lib \
     F:\Qt\Tools\OpenSSL\Win_x64\lib\libcrypto.lib
    }
INCLUDEPATH += F:\Qt\Tools\OpenSSL\Win_x64\include\
