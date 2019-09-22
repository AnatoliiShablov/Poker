cmake_minimum_required(VERSION 3.14)
project(Poker)

set(CMAKE_CXX_STANDARD 17)

add_library(Poker
        src/Card.h
        src/Deck.h
        src/Deck.cpp
        src/Croupier.h
        src/Croupier.cpp
        src/Player.h
        src/Player.cpp
        src/Table.h
        src/Table.cpp)

#add_executable(Poker main.cpp)