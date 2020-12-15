#-------------------------------------------------
#
# Project created by QtCreator 2016-01-20T10:00:38
#
#-------------------------------------------------

QT       += core gui network script

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


#target folder
TARGET = cdos-desktop-cloud 
TEMPLATE = app
DESTDIR = bin/
UI_DIR = forms
RCC_DIR = tmp
MOC_DIR = tmp
OBJECTS_DIR = tmp

INCLUDEPATH += ./movie

#lang
TRANSLATIONS += lang/lang_zh_CN.ts\
                lang/lang_en_US.ts
#source
SOURCES += main.cpp\
    homewindow.cpp \
    vmwidget.cpp \
    logindialog.cpp \
    common.cpp \
    mytitlebar.cpp \
    waitdialog.cpp \
    worker.cpp \
    detaildialog.cpp \
    scale/cursorposcalculator.cpp \
    scale/frameless_helper.cpp \
    scale/widgetdata.cpp \
    qflowlayout.cpp \
    movie/carouselimagewindow.cpp \
    httpdownload/httppicdownload.cpp \
    qmessagehandles.cpp \
    rdesktoptip.cpp \
    setapprelationdialog.cpp

HEADERS  += \
    homewindow.h \
    simple_log.h \
    vmwidget.h \
    logindialog.h \
    common.h \
    mytitlebar.h \
    waitdialog.h \
    worker.h \
    detaildialog.h \
    scale/cursorposcalculator.h \
    scale/frameless_helper.h \
    scale/widgetdata.h \
    common.h \
    qflowlayout.h \
    movie/carouselimagewindow.h \
    httpdownload/httppicdownload.h \
    gwsdk.h \
    qmessagehandles.h \
    rdesktoptip.h \
    setapprelationdialog.h

FORMS    += \
    vmwidget.ui \
    logindialog.ui \
    homewindow.ui \
    detaildialog.ui \
    rdesktoptip.ui \
    setapprelationdialog.ui

RESOURCES += \
    resource.qrc

#CONFIG +=c++11
CONFIG += static
QMAKE_CXXFLAGS_DEBUG += -std=c++11 -static-libgcc 
QMAKE_CXXFLAGS_RELEASE += -std=c++11 -static-libgcc 
DISTFILES +=
