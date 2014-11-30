#-------------------------------------------------
#
# Project created by QtCreator 2013-09-15T16:54:34
#
#-------------------------------------------------

QT       += core gui multimedia widgets sql

TARGET = Apollo
TEMPLATE = app

CONFIG += link_pkgconfig
PKGCONFIG += libchromaprint libavcodec libavformat libavutil libavresample


SOURCES += main.cpp\
        mainwindow.cpp \
    song.cpp \
    tag.cpp \
    tagcreator.cpp \
    id3v2tag.cpp \
    id3v2_3_0tag.cpp \
    id3v2_4_0tag.cpp \
    utils.cpp \
    id3v2tagcreator.cpp \
    chromaprint_api.cpp \
    musicdb.cpp

HEADERS  += mainwindow.h \
    song.h \
    tag.h \
    tagcreator.h \
    id3v2tag.h \
    id3v2_3_0tag.h \
    id3v2_4_0tag.h \
    utils.h \
    id3v2tagcreator.h \
    chromaprint_api.h \
    musicdb.h

FORMS    += mainwindow.ui

QMAKE_CXXFLAGS += -std=c++0x
