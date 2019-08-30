cmake_minimum_required(VERSION 3.14)
project(Poker)

set(CMAKE_CXX_STANDARD 17)


add_library(CardLib cards.h cards.cpp)
#add_executable(Poker main.cpp)