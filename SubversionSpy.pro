#-------------------------------------------------
#
# Project created by QtCreator 2012-07-08T12:37:01
#
#-------------------------------------------------

QT       += core gui xml webkit

TARGET = SubversionSpy
TEMPLATE = app

QMAKE_LFLAGS +=

SOURCES += main.cpp\
        subversionspy.cpp \
    subversionparser.cpp \
    spydb.cpp \
    listenerpathform.cpp \
    threadmonitor.cpp \
    subversionworker.cpp \
    spywkbridge.cpp

HEADERS  += subversionspy.h \
    subversionparser.h \
    spydb.h \
    listenerpathform.h \
    threadmonitor.h \
    subversionworker.h \
    spytypes.h \
    spywkbridge.h

RESOURCES += \
    spyresources.qrc

FORMS += \
    listenerpathform.ui

OTHER_FILES += \
    wkGui/index.html \
    wkGui/styles/main.css \
    wkGui/js/spy.js \
    wkGui/js/nav.js \
    wkGui/js/main.js \
    wkGui/js/repository.js
