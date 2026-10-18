#pragma once
#include <SFML/Graphics.hpp>
#include "../player/Player.h"
#include "../levels/Level.h"

struct Game {
    sf::RenderWindow window{sf::VideoMode(800, 600), "Ilia Zhukov"};
    Player player;
    Level level;
    sf::Clock clock;

    Game();
    void Run();
};