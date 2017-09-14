#-------------------------------------------------
#
# Project created by QtCreator 2016-01-20T10:00:38
#
#-------------------------------------------------

QT       += core gui network script

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


#target folder
TARGET = NfsCloudClient
TEMPLATE = app
DESTDIR = bin/
UI_DIR = forms
RCC_DIR = tmp
MOC_DIR = tmp
OBJECTS_DIR = tmp

#lang
TRANSLATIONS += lang/lang_zh_CN.ts\
                lang/lang_en_US.ts
#source
SOURCES += main.cpp\
    homewindow.cpp \
    configurefile.cpp \
    vmwidget.cpp \
    logindialog.cpp

HEADERS  += \
    homewindow.h \
    configurefile.h \
    simple_log.h \
    vmwidget.h \
    logindialog.h

FORMS    += \
    homewindow.ui \
    vmwidget.ui \
    logindialog.ui

RESOURCES += \
    resource.qrc

#CONFIG +=c++11
CONFIG += static
QMAKE_CXXFLAGS_DEBUG += -std=c++11 -static-libgcc 
QMAKE_CXXFLAGS_RELEASE += -std=c++11 -static-libgcc 
DISTFILES +=
