#pragma once
#include <SFML/Graphics.hpp>
#include "../Map/Map.h"

struct Player {
    sf::Sprite sprite;
    sf::Texture texture;
    sf::Vector2f velocity;
    bool isGrounded;
    bool touchingLeft;
    bool touchingRight;

    Player();
    void ResolveCollisions(const Map& map, float dt);
    void Update(float dt);
    void Draw(sf::RenderWindow& window);
};
