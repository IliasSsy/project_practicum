#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "../Map/Map.h"
#include "../Enemies/Soldier/Soldier.h"

struct Level {
    Map map;
    sf::Texture soldierTexture;
    std::vector<Soldier> soldiers;

    Level();
    void Init();
    void Update(float dt, sf::Vector2f playerPos);
    void Draw(sf::RenderWindow& window);
};