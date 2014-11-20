#!/bin/sh
clang++ -std=c++11 -O0 -Wall -Werror -Wno-deprecated-register -I/usr/include/qt4  OpenGLWidget.cpp codi.cpp -lGLEW -lGL -lQtOpenGL -lQtGui -lQtCore -lPolyVoxCore -o codi2
# && ./codi2
#clang++ -std=c++11 -O0 -g -Wall -Werror codi.cpp -o codi2 && ./codi2
