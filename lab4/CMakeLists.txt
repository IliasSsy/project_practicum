cmake_minimum_required(VERSION 3.10)
project(sfml-platformer-iter1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 2.6 COMPONENTS window graphics system REQUIRED)

add_executable(game
        src/main.cpp
        src/Game/game.cpp
        src/Player/player.cpp
        src/Levels/level.cpp
        src/Map/Map.cpp
        ../lab6/src/main.cpp
        ../lab6/src/Player/Player.cpp
        ../lab6/src/Player/Player.cpp
        ../lab6/src/Map/Map.cpp
        ../lab6/src/Levels/Level.cpp
        ../lab6/src/Game/Game.cpp
        ../lab6/src/main.cpp
)

target_include_directories(game PRIVATE
        src/Game
        src/Player
        src/Levels
        src/Map
)

target_link_libraries(game PRIVATE sfml-graphics sfml-window sfml-system)