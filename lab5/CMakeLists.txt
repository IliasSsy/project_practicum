cmake_minimum_required(VERSION 3.8)
project(sfml-platformer-iter1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 2 COMPONENTS window graphics system REQUIRED)

add_executable(game
        src/main.cpp
        src/Player/Player.cpp
        src/Game/Game.cpp
)

# Добавляем директории объектов в пути поиска заголовочных файлов
target_include_directories(game PRIVATE
        src/Player
        src/Game
)

target_link_libraries(game PRIVATE sfml-graphics sfml-window sfml-system)