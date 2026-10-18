#include "Player.h"
#include <iostream>
#include <algorithm>

constexpr float ACCELERATION = 1800.0f;
constexpr float MAX_SPEED = 300.0f;
constexpr float FRICTION = 0.88f;
constexpr float GRAVITY = 980.0f;
constexpr float JUMP_FORCE = -550.0f;
constexpr float WALL_SLIDE_SPEED = 80.0f;
constexpr float WALL_JUMP_FORCE_X = 400.0f;
constexpr float PLAYER_SCALE = 0.1f;

Player::Player() {
    if (!texture.loadFromFile("../assets/hero.png")) {
        std::cout << "Failed to load texture" << std::endl;
    }
    sprite.setTexture(texture);
    sprite.setScale(PLAYER_SCALE, PLAYER_SCALE);
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.0f, bounds.height);
    sprite.setPosition(100.f, 400.0f);
    velocity = {0.f, 0.f};
    isGrounded = false;
    touchingLeft = false;
    touchingRight = false;
}

void Player::ResolveCollisions(const std::vector<sf::RectangleShape>& platforms) {
    isGrounded = false;
    touchingLeft = false;
    touchingRight = false;
    sf::FloatRect playerBounds = sprite.getGlobalBounds();
    float wallOffsetX = 55.0f;
    sf::FloatRect playerHitbox(
        playerBounds.left + wallOffsetX,
        playerBounds.top,
        playerBounds.width - (wallOffsetX * 2.0f),
        playerBounds.height
    );
    for (size_t i = 0; i < platforms.size(); i++) {
        sf::FloatRect wallBounds = platforms[i].getGlobalBounds();
        if (playerBounds.intersects(wallBounds)) {
            if (velocity.y > 0 && playerBounds.top + playerBounds.height - velocity.y <= wallBounds.top + 5.f) {
                sprite.setPosition(sprite.getPosition().x, wallBounds.top);
                velocity.y = 0.0f;
                isGrounded = true;
            }
            else if (playerHitbox.intersects(wallBounds)) {
                if (playerHitbox.left + playerHitbox.width / 2.0f < wallBounds.left + wallBounds.width / 2.0f) {
                    sprite.setPosition(wallBounds.left - (playerBounds.width / 2.0f) + wallOffsetX, sprite.getPosition().y);
                    touchingRight = true;
                } else {
                    sprite.setPosition(wallBounds.left + wallBounds.width + (playerBounds.width / 2.0f) - wallOffsetX, sprite.getPosition().y);
                    touchingLeft = true;
                }
            }
            playerBounds = sprite.getGlobalBounds();
            playerHitbox.left = playerBounds.left + wallOffsetX;
            playerHitbox.top = playerBounds.top;
        }
    }
}

void Player::Update(float dt, bool touchingLeftSide, bool touchingRightSide) {
    bool isMoving = false;
    float currentAcceleration = isGrounded ? ACCELERATION : ACCELERATION * 0.5f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        velocity.x += currentAcceleration * dt;
        isMoving = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        velocity.x -= currentAcceleration * dt;
        isMoving = true;
    }
    if (isGrounded && !isMoving) {
        velocity.x *= FRICTION;
    }
    velocity.x = std::clamp(velocity.x, -MAX_SPEED, MAX_SPEED);
    if (!isGrounded) {
        if ((touchingLeftSide || touchingRightSide) && velocity.y > 0) {
            velocity.y += (GRAVITY * 0.2f) * dt;
            velocity.y = std::min(velocity.y, WALL_SLIDE_SPEED);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::W)     ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            {
                velocity.y = JUMP_FORCE;
                if (touchingLeftSide) {
                    velocity.x = WALL_JUMP_FORCE_X;
                } else if (touchingRightSide) {
                    velocity.x = -WALL_JUMP_FORCE_X;
                }
            }
        } else {
            velocity.y += GRAVITY * dt;
        }
    } else {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::W)     ||
            sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
        {
            velocity.y = JUMP_FORCE;
            isGrounded = false;
        }
    }
    sprite.move(velocity * dt);
    if (velocity.x > 10.0f) {
        sprite.setScale(PLAYER_SCALE, PLAYER_SCALE);
    } else if (velocity.x < -10.0f) {
        sprite.setScale(-PLAYER_SCALE, PLAYER_SCALE);
    }
}

void Player::Draw(sf::RenderWindow &window) {
    window.draw(sprite);
}