#-------------------------------------------------
#
# Project created by QtCreator 2014-10-10T10:36:40
#
#-------------------------------------------------

QT       += core gui opengl xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ArtefactViewer
TEMPLATE = app

unix:!macx: LIBS += -L/usr/lib/nvidia-340/ -lGL

INCLUDEPATH += /usr/include/nvidia-340
DEPENDPATH += /usr/include/nvidia-340

SOURCES +=\
        avmainwindow.cpp \
    avcontroller.cpp \
    avpluginmanager.cpp \
    avmodel.cpp \
    avabout.cpp \
    avglwidget.cpp \
    avmain.cpp \
    avtrackball.cpp \
    avoffscreendialog.cpp \
    avlight.cpp \
    avpqreader.cpp

HEADERS  += avmainwindow.h \
    avcontroller.h \
    avpluginmanager.h \
    avmodel.h \
    avabout.h \
    avglwidget.h \
    avplugininterfaces.h \
    avtrackball.h \
    avoffscreendialog.h \
    avlight.h \
    PQMTClient.h \
    avpqreader.h \
    avtouchpoint.h \
    avpointframe.h

FORMS    += avmainwindow.ui \
    about.ui \
    avoffscreendialog.ui

RESOURCES += \
    resources.qrc

RC_ICONS = ../icons/AVIcon.ico
	
DESTDIR = ../../bin

#copy the manual to the bin directory
copymanual.commands = $(COPY_DIR) $$shell_path($$PWD/../manual) $$shell_path($$DESTDIR/manual)
first.depends = $(first) copymanual
export(first.depends)
export(copymanual.commands)
QMAKE_EXTRA_TARGETS += first copymanual

win32: LIBS += -L$$PWD/../lib/x64/ -lPQMTClient

INCLUDEPATH += $$PWD/../include
DEPENDPATH += $$PWD/../include

OTHER_FILES += \
    BLAH.txt
