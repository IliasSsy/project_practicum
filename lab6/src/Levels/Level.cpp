#include "Level.h"
#include "../Map/Map.h"
#include <iostream>

Level::Level() {
    MapInit(map);
}

void Level::Init() {
    sf::Image img;
    if (img.loadFromFile("../assets/soldier_spritesheet.png")) {
        soldierTexture.loadFromImage(img);
    } else {
        std::cout << "ERROR: Failed to load soldier spritesheet" << std::endl;
    }

    Soldier s1;
    s1.Init(soldierTexture, {500.0f, 200.0f});
    soldiers.push_back(s1);
}

void Level::Update(float dt, sf::Vector2f playerPos) {
    for (auto& soldier : soldiers) {
        soldier.Update(dt, map, playerPos);
    }
}

void Level::Draw(sf::RenderWindow& window) {
    mapDraw(window, map);
    for (auto& soldier : soldiers) {
        soldier.Draw(window);
    }
}