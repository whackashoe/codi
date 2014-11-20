#!/bin/sh
clang++ -o codi -std=c++11 -O3 -Wall -Wno-deprecated-register  -lsfml-system -lsfml-graphics -lsfml-window -lsfml-audio "codi.cpp" && ./codi