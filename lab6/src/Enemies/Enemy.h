#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

struct Enemy {
    sf::Sprite sprite;
    sf::Vector2f velocity{0.0f, 0.0f};
    float scale = 0.4f;
    bool facingRight = true;
    bool isDead = false;
    int currentFrame = 0;
    float animTimer = 0.0f;
    const std::vector<sf::IntRect>* lastActiveRects = nullptr;
    void InitBase(const sf::Texture& texture, sf::Vector2f startPos);
    void UpdateAnimation(float dt, const std::vector<sf::IntRect>& rects, float frameDuration, bool loop = true);
    void UpdateFacing(float targetX);
    void Draw(sf::RenderWindow& window);
};