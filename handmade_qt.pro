#-------------------------------------------------
#
# Project created by QtCreator 2014-12-25T14:25:16
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

DEFINES +=\
        HANDMADE_INTERNAL
        HANDMADE_SLOW

TARGET = handmade_qt
TEMPLATE = app

SOURCES +=\
        handmadeqt.cpp

HEADERS  +=\
        handmadeqt.h

unix|win32: LIBS += -L$$PWD/build/ -lhandmade

INCLUDEPATH += $$PWD/handmade/code
DEPENDPATH += $$PWD/handmade/code
