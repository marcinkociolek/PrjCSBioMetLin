#-------------------------------------------------
#
# Project created by QtCreator 2016-02-04T21:52:47
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = WaveletEnergyQTV004
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp\
    ../../ProjectsLib/LibMarcin/RedundantWaveletLib.cpp \
    ../../ProjectsLib/LibMarcin/StringFcLib.cpp \
    ../../ProjectsLib/LibMarcin/DispLib.cpp \
    ../../ProjectsLib/LibMarcin/RegionU16Lib.cpp \
    ../../ProjectsLib/tinyxml/tinystr.cpp \
    ../../ProjectsLib/tinyxml/tinyxml.cpp \
    ../../ProjectsLib/tinyxml/tinyxmlerror.cpp \
    ../../ProjectsLib/tinyxml/tinyxmlparser.cpp

HEADERS += \
    ../../ProjectsLib/LibMarcin/RedundantWaveletLib.h \
    ../../ProjectsLib/LibMarcin/StringFcLib.h \
    ../../ProjectsLib/LibMarcin/DispLib.h \
    ../../ProjectsLib/LibMarcin/RegionU16Lib.h \
    ../../ProjectsLib/tinyxml/tinystr.h \
    ../../ProjectsLib/tinyxml/tinyxml.h

INCLUDEPATH += ../../ProjectsLib/tinyxml
INCLUDEPATH += ../../ProjectsLib/LibMarcin
INCLUDEPATH += /usr/include/opencv2
INCLUDEPATH += /usr/include/boost

unix: LIBS += -lopencv_core
unix: LIBS += -lopencv_imgproc
unix: LIBS += -lopencv_highgui
unix: LIBS += -lopencv_contrib

unix: LIBS += -lboost_filesystem
unix: LIBS += -lboost_system
unix: LIBS += -lboost_regex

QMAKE_CXXFLAGS += -std=c++11
