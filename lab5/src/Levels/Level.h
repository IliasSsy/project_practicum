#pragma once
#include <SFML/Graphics.hpp>
#include "vector"

struct Level {
    std::vector<sf::RectangleShape> platform;

    Level();
    void Draw(sf::RenderWindow& window);
};