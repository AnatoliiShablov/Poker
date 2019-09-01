cmake_minimum_required(VERSION 3.14)
project(Poker)

set(CMAKE_CXX_STANDARD 17)


add_library(Card Card.h)
set_target_properties(Card PROPERTIES LINKER_LANGUAGE CXX)

add_library(Deck Deck.h Deck.cpp)
target_link_libraries(Deck Card)

add_library(Player Player.h Player.cpp)
target_link_libraries(Player Card)

add_library(Table Table.h Table.cpp)
target_link_libraries(Table Player Deck)

#add_executable(Poker main.cpp)