#include "iostream"
#include "player.h"

constexpr float ACCELERATION = 1800.0f;
constexpr float MAX_SPEED = 300.0f;
constexpr float FRICTION = 0.88f;

const float dt = 1.f / 60.f;

Player::Player() {
    if (!texture.loadFromFile("assets/hero.png")) {
        std::cout << "Failed to load texture" << std::endl;
    }
    sprite.setTexture(texture);

    sf::Floatrect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);

    sprite.setPosition(100.f, 100.f);

    velocity = {0, 0};
    isGrounded = false;
}

Player::Update() {
    bool isMoving = false;
    if (sf::Keyboar::isKeyPressed(sf::Keyboard::Right) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        velocity.x += ACCELERATION * dt;
        isMoving = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        velocity.x -= ACCELERATION * dt;
        isMoving = true;
    }
    if (!isMoving) {

    }

}

Player::Draw() {
    window.draw(sprite);
}

