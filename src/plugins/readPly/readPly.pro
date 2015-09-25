#-------------------------------------------------
#
# Project created by QtCreator 2014-10-17T16:49:21
#
#-------------------------------------------------

QT       -= gui
QT       += core opengl

TARGET = $$qtLibraryTarget(readPly)
TEMPLATE = lib
CONFIG += plugin

SOURCES += readply.cpp \
    rply.c

HEADERS += readply.h\
    rply.h

INCLUDEPATH  += ../../app

DESTDIR = ../../../bin/plugins

unix:!macx: LIBS += -L/usr/lib/nvidia-340/ -lGL

INCLUDEPATH += /usr/include/nvidia-340
DEPENDPATH += /usr/include/nvidia-340
