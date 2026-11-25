#include "Enemy.h"

void Enemy::InitBase(const sf::Texture& texture, sf::Vector2f startPosition) {
    sprite.setTexture(texture);
    sprite.setPosition(startPosition);
    sprite.setScale(scale, scale);
}

void Enemy::UpdateAnimation(float dt, const std::vector<sf::IntRect> &rects, float frameDuration, bool loop) {
    if (rects.empty()) return;
    if (lastActiveRects != &rects) {
        lastActiveRects = &rects;
        currentFrame = 0;
        animTimer = 0.0f;
    }
    animTimer += dt;
    if (animTimer >= frameDuration) {
        animTimer -= frameDuration;
        if (currentFrame + 1 < static_cast<int>(lastActiveRects->size())) {
            currentFrame++;
        } else if (loop) {
            currentFrame = 0;
        }
    }
    const sf::IntRect& r = rects[currentFrame];
    sprite.setTextureRect(r);
    sprite.setOrigin(r.width / 2.0f, static_cast<float>(r.height));
}

void Enemy::UpdateFacing(float targetX) {
    facingRight = (targetX >= sprite.getPosition().x);
    sprite.setScale(facingRight ? scale : -scale, scale);
}

void Enemy::Draw(sf::RenderWindow& window) {
    window.draw(sprite);
}
