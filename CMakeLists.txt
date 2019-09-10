cmake_minimum_required(VERSION 2.6)

PROJECT(uncle_ben)
INCLUDE(GNUInstallDirs)
ADD_DEFINITIONS(-Os -ggdb -Wall -Werror --std=c++11 -Wmissing-declarations)

SET(SOURCES main.cpp bridge.cpp)
ADD_EXECUTABLE(uncle_ben ${SOURCES})
