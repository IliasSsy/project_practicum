#pragma once
#include <SFML/Graphics.hpp>
#include "../player/Player.h"
#include "../levels/Level.h"

struct Game {
    sf::RenderWindow window{sf::VideoMode(2560, 1664), "Ilia Zhukov"};
    Player player;
    Level level;
    sf::Clock clock;
    sf::View view;

    Game();
    void Run();
};