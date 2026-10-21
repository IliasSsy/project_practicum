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

struct GridRange {
    int startCol, endCol, startRow, endRow;
};

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

    if (!runBuffer.loadFromFile("../assets/music/run.mp3")) {
        std::cout << "ERROR: Failed to load run.mp3" << std::endl;
    } else {
        runSound.setBuffer(runBuffer);
        runSound.setLoop(true);
    }

    if (!fallBuffer.loadFromFile("../assets/music/fall.mp3")) {
        std::cout << "ERROR: Failed to load fall.mp3" << std::endl;
    } else {
        fallSound.setBuffer(fallBuffer);
        fallSound.setLoop(true);
    }

    if (!brakeBuffer.loadFromFile("../assets/music/brake.mp3")) {
        std::cout << "ERROR: Failed to load brake.mp3" << std::endl;
    } else {
        brakeSound.setBuffer(brakeBuffer);
        brakeSound.setLoop(true);
    }
}

void Player::HandleHorizontalMovement(float dt) {
    bool moveLeft = sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A);
    bool moveRight = sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D);

    float currentAcceleration = isGrounded ? ACCELERATION : ACCELERATION * 0.5f;

    if (moveLeft && !moveRight) {
        velocity.x -= currentAcceleration * dt;
        if (isGrounded && runSound.getStatus() != sf::Sound::Playing) runSound.play();
    }
    else if (moveRight && !moveLeft) {
        velocity.x += currentAcceleration * dt;
        if (isGrounded && runSound.getStatus() != sf::Sound::Playing) runSound.play();
    }
    else if (isGrounded) {
        velocity.x *= FRICTION;
        if (std::abs(velocity.x) < 1.0f) {
            velocity.x = 0.0f;
        }
        runSound.pause();
    }

    if (!isGrounded) {
        runSound.pause();
    }

    velocity.x = std::clamp(velocity.x, -MAX_SPEED, MAX_SPEED);
}

void Player::HandleVerticalMovement(float dt, bool jumpPressed) {
    if (isGrounded) {
        fallSound.pause();
        brakeSound.pause();

        if (jumpPressed) {
            velocity.y = JUMP_FORCE;
        }
        return;
    }

    if ((touchingLeft || touchingRight) && velocity.y > 0.0f) {
        velocity.y += GRAVITY * 0.2f * dt;
        velocity.y = std::min(velocity.y, WALL_SLIDE_SPEED);

        if (brakeSound.getStatus() != sf::Sound::Playing) brakeSound.play();
        fallSound.pause();

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
        brakeSound.pause();

        if (velocity.y > 50.0f) {
            if (fallSound.getStatus() != sf::Sound::Playing) fallSound.play();
        } else {
            fallSound.pause();
        }
    }
}

void Player::Update(float dt) {
    if (dt <= 0.0f) return;

    HandleHorizontalMovement(dt);

    bool jumpPressed =
        sf::Keyboard::isKeyPressed(sf::Keyboard::Space) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Up);

    HandleVerticalMovement(dt, jumpPressed);

    if (velocity.x > 1.0f) {
        sprite.setScale(PLAYER_SCALE, PLAYER_SCALE);
    }
    else if (velocity.x < -1.0f) {
        sprite.setScale(-PLAYER_SCALE, PLAYER_SCALE);
    }
}

GridRange GetGridRange(const sf::FloatRect& box, const Map& map) {
    return {
        std::max(0, static_cast<int>(box.left / TILE_SIZE)),
        std::min(static_cast<int>(map.width) - 1, static_cast<int>((box.left + box.width + EPSILON) / TILE_SIZE)),
        std::max(0, static_cast<int>(box.top / TILE_SIZE)),
        std::min(static_cast<int>(map.height) - 1, static_cast<int>((box.top + box.height + EPSILON) / TILE_SIZE))
    };
}

void ResolveXCollisions(sf::Sprite& sprite, sf::Vector2f& velocity,
                        bool& touchingLeft, bool& touchingRight, const Map& map, float dt) {
    sprite.move(velocity.x * dt, 0.0f);
    sf::Vector2f pos = sprite.getPosition();
    touchingLeft = touchingRight = false;
    sf::FloatRect box(pos.x - PLAYER_WIDTH / 2.f, pos.y - PLAYER_HEIGHT, PLAYER_WIDTH, PLAYER_HEIGHT);
    GridRange r = GetGridRange(box, map);

    for (int row = r.startRow; row <= r.endRow; ++row) {
        for (int col = r.startCol; col <= r.endCol; ++col) {
            if (!IsTileSolid(map.grid[row * map.width + col])) continue;
            sf::FloatRect tile(col * TILE_SIZE, row * TILE_SIZE, TILE_SIZE, TILE_SIZE);
            if (!box.intersects(tile)) continue;

            if (velocity.x > 0.0f) {
                pos.x = tile.left - PLAYER_WIDTH / 2.f - EPSILON;
                touchingRight = true;
            } else if (velocity.x < 0.0f) {
                pos.x = tile.left + tile.width + PLAYER_WIDTH / 2.f + EPSILON;
                touchingLeft = true;
            }
            velocity.x = 0.0f;
        }
    }
    sprite.setPosition(pos);
}

void ResolveYCollisions(sf::Sprite& sprite, sf::Vector2f& velocity,
                        bool& isGrounded, const Map& map, float dt) {
    sprite.move(0.0f, velocity.y * dt);
    sf::Vector2f pos = sprite.getPosition();
    isGrounded = false;
    sf::FloatRect box(pos.x - PLAYER_WIDTH / 2.f, pos.y - PLAYER_HEIGHT, PLAYER_WIDTH, PLAYER_HEIGHT + 1.0f);
    GridRange r = GetGridRange(box, map);
    r.endRow = std::min(static_cast<int>(map.height) - 1, static_cast<int>((box.top + box.height) / TILE_SIZE));

    for (int row = r.startRow; row <= r.endRow; ++row) {
        for (int col = r.startCol; col <= r.endCol; ++col) {
            if (!IsTileSolid(map.grid[row * map.width + col])) continue;
            sf::FloatRect tile(col * TILE_SIZE, row * TILE_SIZE, TILE_SIZE, TILE_SIZE);
            if (!box.intersects(tile)) continue;

            if (velocity.y > 0.0f) {
                pos.y = tile.top;
                isGrounded = true;
            } else if (velocity.y < 0.0f) {
                pos.y = tile.top + tile.height + PLAYER_HEIGHT + EPSILON;
            }
            velocity.y = 0.0f;
        }
    }
    sprite.setPosition(pos);
}

void Player::ResolveCollisions(const Map& map, float dt) {
    ResolveXCollisions(sprite, velocity, touchingLeft, touchingRight, map, dt);
    ResolveYCollisions(sprite, velocity, isGrounded, map, dt);
}

void Player::Draw(sf::RenderWindow& window) {
    window.draw(sprite);
}