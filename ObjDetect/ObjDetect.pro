#-------------------------------------------------
#
# Project created by QtCreator 2015-03-07T15:08:19
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS += -std=c++11

TARGET = ObjDetect
TEMPLATE = app

CONFIG += link_pkgconfig
PKGCONFIG += opencv

SOURCES += main.cpp\
        mainwindow.cpp \
    qpiclabel.cpp \
    imagematcher.cpp \
    pricetagdetector.cpp

HEADERS  += mainwindow.h \
    qpiclabel.h \
    imagematcher.h \
    pricetagdetector.h

FORMS    += mainwindow.ui
