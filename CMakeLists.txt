cmake_minimum_required(VERSION 3.14)
project(Poker)

set(CMAKE_CXX_STANDARD 17)

find_package(SFML 2.5 COMPONENTS network graphics window system REQUIRED)

add_library(PokerLib SHARED
        src/lib/Card.h
        src/lib/Card.cpp
        src/lib/Croupier.h
        src/lib/Croupier.cpp
        src/lib/Deck.h
        src/lib/Deck.cpp
        src/lib/Player.h
        src/lib/Player.cpp
        src/lib/Table.h
        src/lib/Table.cpp)

add_executable(GameServer src/server.cpp)
target_include_directories(GameServer PUBLIC ${SFML_INCLUDE_DIR})
target_link_libraries(GameServer PokerLib sfml-system sfml-window sfml-graphics sfml-network)

add_executable(GameClient src/client.cpp)
target_include_directories(GameClient PUBLIC ${SFML_INCLUDE_DIR})
target_link_libraries(GameClient PokerLib sfml-system sfml-window sfml-graphics sfml-network)
