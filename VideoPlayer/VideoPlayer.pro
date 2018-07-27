#-------------------------------------------------
#
# Project created by QtCreator 2018-03-26T19:01:54
#
#-------------------------------------------------

QT       += core gui
QT += multimedia
QT += multimediawidgets
QT += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VideoPlayer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    myhttp.cpp \
    setdialog.cpp

HEADERS  += mainwindow.h \
    myhttp.h \
    setdialog.h

FORMS    += mainwindow.ui \
    setdialog.ui

RESOURCES += \
    VideoPlayer.qrc
