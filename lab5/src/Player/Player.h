#pragma once
#include <SFML/Graphics.hpp>

struct Player {
    sf::Sprite sprite;
    sf::Texture texture;
    sf::Vector2f velocity;
    bool isGrounded;
    bool touchingLeft;
    bool touchingRight;

    Player();
    void ResolveCollisions(const std::vector<sf::RectangleShape>& platforms);
    void Update(float dt, bool touchingLeftSlide, bool touchingRightSlide);
    void Draw(sf::RenderWindow& window);
};