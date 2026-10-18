#include "Player.h"
#include <iostream>
#include <algorithm>
#include <cmath>

constexpr float ACCELERATION = 1800.0f;
constexpr float MAX_SPEED = 300.0f;
constexpr float FRICTION = 0.88f;
constexpr float GRAVITY = 980.0f;
constexpr float JUMP_FORCE = -550.0f;
constexpr float WALL_SLIDE_SPEED = 80.0f;
constexpr float WALL_JUMP_FORCE_X = 400.0f;
constexpr float PLAYER_SCALE = 0.1f;

constexpr float PLAYER_WIDTH = 24.0f;
constexpr float PLAYER_HEIGHT = 48.0f;
constexpr float EPSILON = 0.001f;

bool IsTileSolid(TileType type) {
    return type != TileType::Empty;
}

Player::Player() {
    if (!texture.loadFromFile("../assets/hero.png")) {
        std::cout << "Failed to load texture" << std::endl;
    }
    sprite.setTexture(texture);
    sprite.setScale(PLAYER_SCALE, PLAYER_SCALE);
    sf::FloatRect bounds = sprite.getLocalBounds();
    sprite.setOrigin(bounds.width / 2.0f, bounds.height);
    sprite.setPosition(200.0f, 100.0f);
    velocity = {0.0f, 0.0f};
    isGrounded = false;
    touchingLeft = false;
    touchingRight = false;
}

void HandleHorizontalMovement(sf::Vector2f& velocity, bool isGrounded, float dt) {
    bool moveLeft =
        sf::Keyboard::isKeyPressed(sf::Keyboard::Left) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::A);
    bool moveRight =
        sf::Keyboard::isKeyPressed(sf::Keyboard::Right) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::D);
    float currentAcceleration =
        isGrounded ? ACCELERATION : ACCELERATION * 0.5f;
    if (moveLeft && !moveRight) {
        velocity.x -= currentAcceleration * dt;
    }
    else if (moveRight && !moveLeft) {
        velocity.x += currentAcceleration * dt;
    }
    else if (isGrounded) {
        velocity.x *= FRICTION;
        if (std::abs(velocity.x) < 1.0f) {
            velocity.x = 0.0f;
        }
    }
    velocity.x = std::clamp(velocity.x, -MAX_SPEED, MAX_SPEED);
}

void HandleVerticalMovement(sf::Vector2f& velocity, bool isGrounded, bool touchingLeft,
                            bool touchingRight, float dt, bool jumpPressed ) {
    if (isGrounded) {
        if (jumpPressed) {
            velocity.y = JUMP_FORCE;
        }
        return;
    }
    if ((touchingLeft || touchingRight) && velocity.y > 0.0f) {
        velocity.y += GRAVITY * 0.2f * dt;
        velocity.y = std::min(velocity.y, WALL_SLIDE_SPEED);

        if (jumpPressed) {
            velocity.y = JUMP_FORCE;

            if (touchingLeft) {
                velocity.x = WALL_JUMP_FORCE_X;
            }
            else if (touchingRight) {
                velocity.x = -WALL_JUMP_FORCE_X;
            }
        }
    }
    else {
        velocity.y += GRAVITY * dt;
    }
}

void Player::Update(float dt) {
    if (dt <= 0.0f) {
        return;
    }

    HandleHorizontalMovement(velocity, isGrounded, dt);

    bool jumpPressed =
        sf::Keyboard::isKeyPressed(sf::Keyboard::Space) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Up);

    HandleVerticalMovement(
        velocity,
        isGrounded,
        touchingLeft,
        touchingRight,
        dt,
        jumpPressed
    );

    if (velocity.x > 1.0f) {
        sprite.setScale(PLAYER_SCALE, PLAYER_SCALE);
    }
    else if (velocity.x < -1.0f) {
        sprite.setScale(-PLAYER_SCALE, PLAYER_SCALE);
    }
}

void Player::ResolveCollisions(const Map& map, float dt)  {
    sprite.move(velocity.x * dt, 0.0f);

    sf::Vector2f pos = sprite.getPosition();
    touchingLeft = false;
    touchingRight = false;

    sf::FloatRect boundsX(
        pos.x - PLAYER_WIDTH / 2.0f,
        pos.y - PLAYER_HEIGHT,
        PLAYER_WIDTH,
        PLAYER_HEIGHT
    );

    int startCol = std::max(0, static_cast<int>(std::floor(boundsX.left / TILE_SIZE)));
    int endCol = std::min(
        static_cast<int>(map.width) - 1,
        static_cast<int>(std::floor((boundsX.left + boundsX.width - EPSILON) / TILE_SIZE))
    );
    int startRow = std::max(0, static_cast<int>(std::floor(boundsX.top / TILE_SIZE)));
    int endRow = std::min(
        static_cast<int>(map.height) - 1,
        static_cast<int>(std::floor((boundsX.top + boundsX.height - EPSILON) / TILE_SIZE))
    );

    for (int row = startRow; row <= endRow; ++row) {
        for (int col = startCol; col <= endCol; ++col) {
            uint32_t index = static_cast<uint32_t>(row) * map.width + static_cast<uint32_t>(col);
            if (!IsTileSolid(map.grid[index])) {
                continue;
            }

            sf::FloatRect tileBox(
                static_cast<float>(col * TILE_SIZE),
                static_cast<float>(row * TILE_SIZE),
                static_cast<float>(TILE_SIZE),
                static_cast<float>(TILE_SIZE)
            );

            sf::FloatRect playerBox(
                pos.x - PLAYER_WIDTH / 2.0f,
                pos.y - PLAYER_HEIGHT,
                PLAYER_WIDTH,
                PLAYER_HEIGHT
            );

            if (!playerBox.intersects(tileBox)) {
                continue;
            }

            if (velocity.x > 0.0f) {
                pos.x = tileBox.left - PLAYER_WIDTH / 2.0f - EPSILON;
                touchingRight = true;
                velocity.x = 0.0f;
            }
            else if (velocity.x < 0.0f) {
                pos.x = tileBox.left + tileBox.width + PLAYER_WIDTH / 2.0f + EPSILON;
                touchingLeft = true;
                velocity.x = 0.0f;
            }
        }
    }

    sprite.setPosition(pos);
    sprite.move(0.0f, velocity.y * dt);
    pos = sprite.getPosition();
    isGrounded = false;
    sf::FloatRect boundsY(
        pos.x - PLAYER_WIDTH / 2.0f,
        pos.y - PLAYER_HEIGHT,
        PLAYER_WIDTH,
        PLAYER_HEIGHT
    );
    startCol = std::max(0, static_cast<int>(std::floor(boundsY.left / TILE_SIZE)));
    endCol = std::min(
        static_cast<int>(map.width) - 1,
        static_cast<int>(std::floor((boundsY.left + boundsY.width - EPSILON) / TILE_SIZE))
    );
    startRow = std::max(0, static_cast<int>(std::floor(boundsY.top / TILE_SIZE)));
    endRow = std::min(
        static_cast<int>(map.height) - 1,
        static_cast<int>(std::floor((boundsY.top + boundsY.height - EPSILON) / TILE_SIZE))
    );
    for (int row = startRow; row <= endRow; ++row) {
        for (int col = startCol; col <= endCol; ++col) {
            uint32_t index = static_cast<uint32_t>(row) * map.width + static_cast<uint32_t>(col);
            if (!IsTileSolid(map.grid[index])) {
                continue;
            }

            sf::FloatRect tileBox(
                static_cast<float>(col * TILE_SIZE),
                static_cast<float>(row * TILE_SIZE),
                static_cast<float>(TILE_SIZE),
                static_cast<float>(TILE_SIZE)
            );

            sf::FloatRect playerBox(
                pos.x - PLAYER_WIDTH / 2.0f,
                pos.y - PLAYER_HEIGHT,
                PLAYER_WIDTH,
                PLAYER_HEIGHT
            );

            if (!playerBox.intersects(tileBox)) {
                continue;
            }

            if (velocity.y > 0.0f) {
                pos.y = tileBox.top;
                velocity.y = 0.0f;
                isGrounded = true;
            }
            else if (velocity.y < 0.0f) {
                pos.y = tileBox.top + tileBox.height + PLAYER_HEIGHT;
                velocity.y = 0.0f;
            }
        }
    }

    sprite.setPosition(pos);
}

void Player::Draw(sf::RenderWindow& window) {
    window.draw(sprite);
}