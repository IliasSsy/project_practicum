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

void MapInit(Map& map) {
    if (!map.tilesetTexture.loadFromFile("../assets/spritesheet.jpg")) {
        std::cout << "Failed to load spritesheet.png" << std::endl;
    }
    std::ifstream file("../assets/map.txt");
    if (!file.is_open()) {
        std::cout << "Failed to load map.txt" << std::endl;
        return;
    }
    map.width = 60;
    map.height = 27;
    map.grid.clear();
    std::string line;
    for (uint32_t row = 0; row < map.height; ++row) {
        if (!std::getline(file, line)) {
            std::cout << "Not enough rows in map file! Row count stops at: " << row << std::endl;
            return;
        }
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (line.size() != map.width) {
            std::cout << "Invalid map row width at row " << row
                      << " (Expected " << map.width << ", got " << line.size() << ")" << std::endl;
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
                default:  map.grid.push_back(TileType::Empty); break;
            }
        }
    }
}

void mapDraw(sf::RenderWindow& window, const Map& map) {
    sf::Sprite tileSprite(map.tilesetTexture);
    for (uint32_t row = 0; row < map.height; ++row) {
        for (uint32_t col = 0; col < map.width; ++col) {
            uint32_t index = row * map.width + col;
            TileType currentTile = map.grid[index];
            if (currentTile == TileType::Empty) {
                continue;
            }
            switch (currentTile) {
                case TileType::WallMetal:
                    tileSprite.setTextureRect(sf::IntRect(0, TILE_SIZE, TILE_SIZE, TILE_SIZE));
                    break;
                case TileType::Floor:
                    tileSprite.setTextureRect(sf::IntRect(0, TILE_SIZE * 4, TILE_SIZE, TILE_SIZE));
                    break;
                case TileType::Ceiling:
                    tileSprite.setTextureRect(sf::IntRect(TILE_SIZE * 2, 0, TILE_SIZE, TILE_SIZE));
                    break;
                case TileType::Pipe:
                    tileSprite.setTextureRect(sf::IntRect(TILE_SIZE * 8, 0, TILE_SIZE, TILE_SIZE));
                    break;
                case TileType::Spike:
                    tileSprite.setTextureRect(sf::IntRect(1354, 750, 150, 140));
                    break;
                case TileType::Door:
                    tileSprite.setTextureRect(sf::IntRect(TILE_SIZE, TILE_SIZE * 11, TILE_SIZE, TILE_SIZE));
                    break;
            }
            float x = static_cast<float>(col * TILE_SIZE);
            float y = static_cast<float>(row * TILE_SIZE);
            tileSprite.setPosition({x, y});
            window.draw(tileSprite);
        }
    }
}