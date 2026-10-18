#pragma once
#include <SFML/Graphics.hpp>
#include "vector"
#include "Map.h"

struct Level {
    Map map;

    Level();
    void Draw(sf::RenderWindow& window);
};