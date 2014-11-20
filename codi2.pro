CONFIG += qt debug

QMAKE_CXX = clang++
QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS += -O3
QMAKE_CXXFLAGS += -Wall
QMAKE_CXXFLAGS += -Wno-deprecated-register

INCLUDEPATH += /usr/include/qt4

LIBS += -lGLEW
LIBS += -lGL
LIBS += -lQtOpenGL
LIBS += -lQtGui
LIBS += -lQtCore
LIBS += -lPolyVoxCore 

HEADERS += utility.hpp
HEADERS += cell_type.hpp
HEADERS += cell.hpp
HEADERS += network.hpp
HEADERS += OpenGLWidget.h

SOURCES += OpenGLWidget.cpp
SOURCES += codi.cpp
