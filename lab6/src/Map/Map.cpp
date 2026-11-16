#include "Map.h"
#include <iostream>
#include <fstream>
#include <string>

TileType MapGetTile(const Map& map, float worldX, float worldY) {
    if (worldX < 0.f || worldY < 0.f) {
        return TileType::Empty;
    }

    uint32_t col = static_cast<uint32_t>(worldX) / TILE_SIZE;
    uint32_t row = static_cast<uint32_t>(worldY) / TILE_SIZE;

    if (col >= map.width || row >= map.height) {
        return TileType::Empty;
    }

    uint32_t index = row * map.width + col;
    return map.grid[index];
}

bool MapIsSolid(const Map& map, float worldX, float worldY) {
    TileType tile = MapGetTile(map, worldX, worldY);
    if (tile == TileType::WallMetal ||
        tile == TileType::Floor ||
        tile == TileType::Ceiling ||
        tile == TileType::BreakableFloor ||
        tile == TileType::Door)
    {
        return true;
    }
    return false;
}

void MapInit(Map& map) {
    if (!map.tilesetTexture.loadFromFile("../assets/spritesheet.png")) {
        std::cout << "Failed to load spritesheet.png\n";
    }
    if (!map.bgTexture.loadFromFile("../assets/lab1/background.png")) {
        std::cout << "Failed to load background.jpg\n";
    }
    map.bgSprite.setTexture(map.bgTexture);
    float bgScaleX = static_cast<float>(60 * TILE_SIZE) / map.bgTexture.getSize().x;
    float bgScaleY = static_cast<float>(26 * TILE_SIZE) / map.bgTexture.getSize().y;
    map.bgSprite.setScale(bgScaleX, bgScaleY);
    std::ifstream file("../assets/map.txt");
    if (!file.is_open()) {
        std::cout << "Failed to load map.txt\n";
        return;
    }
    map.width = 60;
    map.height = 26;
    map.grid.clear();
    std::string line;
    for (uint32_t row = 0; row < map.height; ++row) {
        if (!std::getline(file, line)) {
            std::cout << "Not enough rows in map file!\n";
            return;
        }
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (line.size() != map.width) {
            std::cout << "Invalid map row width\n";
            return;
        }
        for (uint32_t col = 0; col < map.width; ++col) {
            char symbol = line[col];
            switch (symbol) {
                case '0':
                case '.': map.grid.push_back(TileType::Empty); break;
                case 'F': map.grid.push_back(TileType::Floor); break;
                case 'W': map.grid.push_back(TileType::WallMetal); break;
                case 'C': map.grid.push_back(TileType::Ceiling); break;
                case 'P': map.grid.push_back(TileType::Pipe); break;
                case 'S': map.grid.push_back(TileType::Spike); break;
                case 'D': map.grid.push_back(TileType::Door); break;
                case 'B': map.grid.push_back(TileType::BreakableFloor); break;
                case 'V': map.grid.push_back(TileType::Voice); break;
                default:  map.grid.push_back(TileType::Empty); break;
            }
        }
    }
}

void mapDraw(sf::RenderWindow& window, const Map& map) {
    window.draw(map.bgSprite);
    sf::Sprite tileSprite(map.tilesetTexture);
    tileSprite.setOrigin(0.0f, 0.0f);
    for (uint32_t row = 0; row < map.height; ++row) {
        for (uint32_t col = 0; col < map.width; ++col) {
            uint32_t index = row * map.width + col;
            TileType currentTile = map.grid[index];
            if (currentTile == TileType::Empty) {
                continue;
            }
            sf::IntRect textureRect;
            switch (currentTile) {
                case TileType::WallMetal:
                    textureRect = sf::IntRect(1050, 390, 90, 207);
                    break;
                case TileType::Floor:
                    textureRect = sf::IntRect(690, 450, 150, 150);
                    break;
                case TileType::Ceiling:
                    textureRect = sf::IntRect(300, 0, 150, 150);
                    break;
                case TileType::BreakableFloor:
                    textureRect = sf::IntRect(300, 150, 150, 150);
                    break;
                case TileType::Pipe:
                    textureRect = sf::IntRect(450, 0, 150, 150);
                    break;
                case TileType::Spike:
                    textureRect = sf::IntRect(0, 150, 148, 126);
                    break;
                case TileType::Door:
                    textureRect = sf::IntRect(150, 150, 150, 150);
                    break;
                case TileType::Voice:
                    textureRect = sf::IntRect(1810, 1940, 50, 97);
                    break;
                default:
                    continue;
            }

            tileSprite.setTextureRect(textureRect);

            float scaleX = static_cast<float>(TILE_SIZE) / textureRect.width;
            float scaleY = static_cast<float>(TILE_SIZE) / textureRect.height;
            tileSprite.setScale(scaleX, scaleY);

            float x = static_cast<float>(col * TILE_SIZE);
            float y = static_cast<float>(row * TILE_SIZE);

            tileSprite.setPosition(x, y);

            window.draw(tileSprite);
        }
    }
}