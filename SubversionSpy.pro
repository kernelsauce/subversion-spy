#-------------------------------------------------
#
# Project created by QtCreator 2012-07-08T12:37:01
#
#-------------------------------------------------

QT       += core gui xml qt-gif

TARGET = SubversionSpy
TEMPLATE = app

QMAKE_LFLAGS += -static-libgcc

SOURCES += main.cpp\
        subversionspy.cpp \
    subversionparser.cpp \
    spydb.cpp \
    listenerpathform.cpp \
    threadmonitor.cpp \
    subversionworker.cpp

HEADERS  += subversionspy.h \
    subversionparser.h \
    spydb.h \
    listenerpathform.h \
    threadmonitor.h \
    subversionworker.h \
    spytypes.h

RESOURCES += \
    spyresources.qrc

FORMS += \
    listenerpathform.ui
