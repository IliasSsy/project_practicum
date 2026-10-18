#include "Level.h"
#include "../Map/Map.h"

Level::Level() {
    MapInit(map);
}

void Level::Draw(sf::RenderWindow& window) {
    mapDraw(window, map);
}
