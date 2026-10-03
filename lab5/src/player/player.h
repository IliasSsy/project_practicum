#pragma once
#include <SFML/Graphics.hpp>

struct Player {
    sf::Sprite sprite;
    sf::Texture texture;
    sf::Vector2f velocity;
    bool isGrounded;

    Player();
    void Update(float dt);
    void Draw(sf::RenderWindow& window);
};