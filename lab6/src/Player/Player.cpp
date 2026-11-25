#include "Player.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>

constexpr float ACCELERATION = 1800.0f;
constexpr float MAX_SPEED = 300.0f;
constexpr float FRICTION = 0.88f;
constexpr float GRAVITY = 980.0f;
constexpr float JUMP_FORCE = -550.0f;
constexpr float WALL_SLIDE_SPEED = 80.0f;
constexpr float WALL_JUMP_FORCE_X = 400.0f;
constexpr float PLAYER_SCALE = 0.3f;
constexpr float PLAYER_WIDTH = 32.0f;
constexpr float PLAYER_HEIGHT = 90.0f;
constexpr float RUN_START_SPEED = 35.0f;
constexpr float RUN_STOP_SPEED = 15.0f;
constexpr float EPSILON = 0.001f;

struct GridRange {
    int startCol, endCol, startRow, endRow;
};

bool IsTileSolid(TileType type) {
    return type != TileType::Empty && type != TileType::Voice && type != TileType::Clock;
}

Player::Player(sf::Vector2f startPos)
    : spawnPosition(startPos)
{
    if (!texture.loadFromFile("../assets/hero_spritesheet.png")) {
        std::cout << "Failed to load hero_spritesheet!" << std::endl;
    }
    sprite.setTexture(texture);
    sprite.setScale(PLAYER_SCALE, PLAYER_SCALE);

    idleRects = {
        {129, 44, 140, 293},
        {415, 44, 140, 293},
        {701, 44, 140, 293},
        {986, 44, 140, 293}
    };

    runRects = {
        {109, 386, 204, 276},
        {406, 381, 159, 287},
        {691, 390, 202, 275},
        {969, 387, 200, 283},
        {1255, 384, 203, 283},
        {1535, 385, 203, 283}
    };

    jumpRects = {
        {136, 710, 145, 300},
        {405, 679, 162, 304},
        {699, 721, 192, 276},
        {998, 749, 220, 262}
    };

    wallSlideRects = {
        {1488, 722, 184, 276},
        {1804, 731, 183, 276},
        {2120, 732, 183, 276},
    };

    sprite.setTextureRect(idleRects[0]);
    sprite.setOrigin(idleRects[0].width / 2.0f, idleRects[0].height - (PLAYER_HEIGHT / 2.0f / PLAYER_SCALE));
    sprite.setPosition(spawnPosition);

    hp = maxHp;
    velocity = {0.0f, 0.0f};
    isGrounded = false;
    touchingLeft = false;
    touchingRight = false;
    isFinished = false;
    currentFrame = 0;
    animTimer = 0.0f;
    lastActiveRects = nullptr;

    isRunLoaded = runBuffer.loadFromFile("../assets/music/run.mp3");
    if (isRunLoaded) {
        runSound.setBuffer(runBuffer);
        runSound.setLoop(true);
    } else {
        std::cout << "ERROR: Failed to load run.mp3" << std::endl;
    }

    isFallLoaded = fallBuffer.loadFromFile("../assets/music/fall.mp3");
    if (isFallLoaded) {
        fallSound.setBuffer(fallBuffer);
    } else {
        std::cout << "ERROR: Failed to load fall.mp3" << std::endl;
    }

    isBrakeLoaded = brakeBuffer.loadFromFile("../assets/music/brake.mp3");
    if (isBrakeLoaded) {
        brakeSound.setBuffer(brakeBuffer);
    } else {
        std::cout << "ERROR: Failed to load brake.mp3" << std::endl;
    }
}

void Player::HandleHorizontalMovement(float dt) {
    bool moveLeft = sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A);
    bool moveRight = sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D);
    float acceleration = isGrounded ? ACCELERATION : ACCELERATION * 0.5f;
    if (moveLeft && !moveRight) {
        velocity.x -= acceleration * dt;
    }
    else if (moveRight && !moveLeft) {
        velocity.x += acceleration * dt;
    }
    else if (isGrounded) {
        velocity.x *= std::pow(FRICTION, dt * 60.0f);
        if (std::abs(velocity.x) < 1.0f) {
            velocity.x = 0.0f;
        }
    }

    velocity.x = std::clamp(velocity.x, -MAX_SPEED, MAX_SPEED);

    if (isGrounded && std::abs(velocity.x) > RUN_STOP_SPEED) {
        if (isRunLoaded && runSound.getStatus() != sf::Sound::Playing) {
            runSound.play();
        }
    } else {
        runSound.pause();
    }
}

void Player::HandleVerticalMovement(float dt, bool jumpPressed) {
    if (isGrounded) {
        brakeSound.stop();
        if (jumpPressed) {
            velocity.y = JUMP_FORCE;
            isGrounded = false;
        }
        return;
    }
    if ((touchingLeft || touchingRight) && velocity.y > 0.0f) {
        velocity.y += GRAVITY * -0.2f * dt;
        velocity.y = std::min(velocity.y, WALL_SLIDE_SPEED);

        if (isBrakeLoaded && brakeSound.getStatus() != sf::Sound::Playing) {
            brakeSound.play();
        }

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
        brakeSound.stop();
    }
}

void Player::Update(float dt) {
    if (dt <= 0.0f) return;
    dt = std::min(dt, 0.05f);

    HandleHorizontalMovement(dt);

    bool jumpPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Space) ||
                       sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
                       sf::Keyboard::isKeyPressed(sf::Keyboard::Up);

    HandleVerticalMovement(dt, jumpPressed);

    animTimer += dt;

    float speed = std::abs(velocity.x);
    float frameDuration = 0.1f;
    bool loopAnimation = true;

    const std::vector<sf::IntRect>* activeRects = &idleRects;

    if (!isGrounded) {
        if ((touchingLeft || touchingRight) && !wallSlideRects.empty()) {
            activeRects = &wallSlideRects;
            loopAnimation = true;
        }
        else if (!jumpRects.empty()) {
            activeRects = &jumpRects;
            loopAnimation = false;
        }
        else {
            activeRects = &idleRects;
        }
    }
    else {
        bool wasRunning = (lastActiveRects == &runRects);
        if (!wasRunning && speed >= RUN_START_SPEED) {
            wasRunning = true;
        }
        else if (wasRunning && speed <= RUN_STOP_SPEED) {
            wasRunning = false;
        }
        if (wasRunning && !runRects.empty()) {
            activeRects = &runRects;
            frameDuration = 0.09f;
        }
        else {
            activeRects = &idleRects;
            frameDuration = 0.15f;
        }
    }

    if (activeRects->empty()) {
        activeRects = &idleRects;
        loopAnimation = true;
    }

    if (activeRects->empty()) return;

    if (activeRects != lastActiveRects) {
        lastActiveRects = activeRects;
        currentFrame = 0;
        animTimer = 0.0f;
    }

    if (activeRects == &jumpRects && jumpRects.size() >= 4) {
        if (velocity.y < -420.0f && animTimer < 0.08f) {
            currentFrame = 0;
        }
        else if (velocity.y < -80.0f) {
            currentFrame = 1;
        }
        else if (velocity.y <= 120.0f) {
            currentFrame = 2;
        }
        else {
            currentFrame = 3;
        }
    }
    else {
        while (animTimer >= frameDuration) {
            animTimer -= frameDuration;
            currentFrame++;
            if (currentFrame >= static_cast<int>(activeRects->size())) {
                if (loopAnimation) {
                    currentFrame = 0;
                } else {
                    currentFrame = static_cast<int>(activeRects->size()) - 1;
                }
            }
        }
    }

    sf::IntRect rect = (*activeRects)[currentFrame];
    sprite.setTextureRect(rect);

    float feetOffset = rect.height - (PLAYER_HEIGHT / 2.0f / PLAYER_SCALE);
    if (activeRects == &jumpRects) {
        feetOffset += 25.0f;
    }
    sprite.setOrigin(rect.width / 2.0f, feetOffset);

    if (velocity.x > 1.0f) {
        sprite.setScale(PLAYER_SCALE, PLAYER_SCALE);
    }
    else if (velocity.x < -1.0f) {
        sprite.setScale(-PLAYER_SCALE, PLAYER_SCALE);
    }
}

GridRange GetGridRange(const sf::FloatRect& box, const Map& map) {
    return {
        std::max(0, static_cast<int>(std::floor(box.left / TILE_SIZE))),
        std::min(static_cast<int>(map.width) - 1, static_cast<int>(std::floor((box.left + box.width - EPSILON) / TILE_SIZE))),
        std::max(0, static_cast<int>(std::floor(box.top / TILE_SIZE))),
        std::min(static_cast<int>(map.height) - 1, static_cast<int>(std::floor((box.top + box.height - EPSILON) / TILE_SIZE)))
    };
}

sf::Vector2f GetPlayerSize(const sf::Sprite& sprite) {
    return {PLAYER_WIDTH, PLAYER_HEIGHT};
}

void ResolveXCollisions(sf::Sprite& sprite, sf::Vector2f& velocity, bool& touchingLeft, bool& touchingRight, int& hp, bool& isFinished, const Map& map, float dt) {
    dt = std::min(dt, 0.05f);
    sprite.move(velocity.x * dt, 0.0f);

    sf::Vector2f pos = sprite.getPosition();
    sf::Vector2f size = GetPlayerSize(sprite);

    touchingLeft = false;
    touchingRight = false;
    float shrinkY = 4.0f;

    sf::FloatRect box(
        pos.x - size.x / 2.0f,
        pos.y - size.y / 2.0f + shrinkY,
        size.x,
        size.y - shrinkY * 2.0f
    );

    GridRange range = GetGridRange(box, map);

    for (int row = range.startRow; row <= range.endRow; ++row) {
        for (int col = range.startCol; col <= range.endCol; ++col) {
            TileType currentTile = map.grid[row * map.width + col];
            if (!IsTileSolid(currentTile)) continue;

            sf::FloatRect tile(col * TILE_SIZE, row * TILE_SIZE, TILE_SIZE, TILE_SIZE);
            if (!box.intersects(tile)) continue;

            if (currentTile == TileType::Door) {
                isFinished = true;
                continue;
            }

            if (currentTile == TileType::Spike) {
                hp -= 10;
                velocity.x = (velocity.x > 0.0f) ? -300.0f : 300.0f;
                velocity.y = -400.0f;
                continue;
            }

            if (velocity.x > 0.0f) {
                pos.x = tile.left - size.x / 2.0f - EPSILON;
                touchingRight = true;
            }
            else if (velocity.x < 0.0f) {
                pos.x = tile.left + tile.width + size.x / 2.0f + EPSILON;
                touchingLeft = true;
            }

            velocity.x = 0.0f;
            box.left = pos.x - size.x / 2.0f;
        }
    }

    sprite.setPosition(pos);
}

void ResolveYCollisions(sf::Sprite& sprite, sf::Vector2f& velocity, bool& isGrounded, int& hp, bool& isFinished, const Map& map, float dt) {
    dt = std::min(dt, 0.05f);
    sprite.move(0.0f, velocity.y * dt);

    sf::Vector2f pos = sprite.getPosition();
    sf::Vector2f size = GetPlayerSize(sprite);

    isGrounded = false;
    float shrinkX = 4.0f;

    sf::FloatRect box(
        pos.x - size.x / 2.0f + shrinkX,
        pos.y - size.y / 2.0f,
        size.x - shrinkX * 2.0f,
        size.y
    );

    GridRange range = GetGridRange(box, map);

    for (int row = range.startRow; row <= range.endRow; ++row) {
        for (int col = range.startCol; col <= range.endCol; ++col) {
            TileType currentTile = map.grid[row * map.width + col];
            if (!IsTileSolid(currentTile)) continue;

            sf::FloatRect tile(col * TILE_SIZE, row * TILE_SIZE, TILE_SIZE, TILE_SIZE);
            if (!box.intersects(tile)) continue;

            if (currentTile == TileType::Door) {
                isFinished = true;
                continue;
            }

            if (currentTile == TileType::Spike) {
                hp -= 50;
                velocity.y = -500.0f;
                continue;
            }

            if (velocity.y > 0.0f) {
                pos.y = tile.top - size.y / 2.0f - EPSILON;
                isGrounded = true;
            }
            else if (velocity.y < 0.0f) {
                pos.y = tile.top + tile.height + size.y / 2.0f + EPSILON;
            }

            velocity.y = 0.0f;
            box.top = pos.y - size.y / 2.0f;
        }
    }

    if (!isGrounded && velocity.y >= 0.0f) {
        sf::FloatRect groundProbe(
            pos.x - size.x / 2.0f + shrinkX,
            pos.y + size.y / 2.0f,
            size.x - shrinkX * 2.0f,
            2.0f
        );

        GridRange probeRange = GetGridRange(groundProbe, map);

        for (int row = probeRange.startRow; row <= probeRange.endRow; ++row) {
            for (int col = probeRange.startCol; col <= probeRange.endCol; ++col) {
                TileType tileType = map.grid[row * map.width + col];
                if (!IsTileSolid(tileType)) {
                    continue;
                }

                sf::FloatRect tile(col * TILE_SIZE, row * TILE_SIZE, TILE_SIZE, TILE_SIZE);
                if (groundProbe.intersects(tile)) {
                    isGrounded = true;
                    velocity.y = 0.0f;
                    break;
                }
            }
            if (isGrounded) break;
        }
    }

    sprite.setPosition(pos);
}

void Player::ResolveCollisions(const Map& map, float dt) {
    bool wasGrounded = isGrounded;

    ResolveXCollisions(sprite, velocity, touchingLeft, touchingRight, hp, isFinished, map, dt);
    ResolveYCollisions(sprite, velocity, isGrounded, hp, isFinished, map, dt);

    if (!wasGrounded && isGrounded) {
        if (isFallLoaded) {
            fallSound.play();
        }
    }
}

bool Player::IsNearVoice(const Map& map) const {
    sf::Vector2f pos = sprite.getPosition();
    sf::Vector2f size = GetPlayerSize(sprite);

    sf::FloatRect box(
        pos.x - size.x / 2.0f - 15.0f,
        pos.y - size.y / 2.0f - 15.0f,
        size.x + 30.0f,
        size.y + 30.0f
    );

    GridRange range = GetGridRange(box, map);
    for (int row = range.startRow; row <= range.endRow; ++row) {
        for (int col = range.startCol; col <= range.endCol; ++col) {
            if (map.grid[row * map.width + col] == TileType::Voice) {
                return true;
            }
        }
    }
    return false;
}

bool Player::TryPickupVoice(Map& map) {
    sf::Vector2f pos = sprite.getPosition();
    sf::Vector2f size = GetPlayerSize(sprite);
    sf::FloatRect box(
        pos.x - size.x / 2.0f - 15.0f,
        pos.y - size.y / 2.0f - 15.0f,
        size.x + 30.0f,
        size.y + 30.0f
    );
    GridRange range = GetGridRange(box, map);
    for (int row = range.startRow; row <= range.endRow; ++row) {
        for (int col = range.startCol; col <= range.endCol; ++col) {
            int index = row * map.width + col;
            if (map.grid[index] == TileType::Voice) {
                map.grid[index] = TileType::Empty;
                return true;
            }
        }
    }
    return false;
}

void Player::SetSpawnPosition(sf::Vector2f pos) {
    spawnPosition = pos;
    sprite.setPosition(spawnPosition);
}

void Player::Reset() {
    sprite.setPosition(spawnPosition);
    hp = maxHp;
    velocity = {0.0f, 0.0f};
    isGrounded = false;
    touchingLeft = false;
    touchingRight = false;
    isFinished = false;
    animTimer = 0.0f;
    currentFrame = 0;
    lastActiveRects = nullptr;
    runSound.stop();
    fallSound.stop();
    brakeSound.stop();

    sprite.setTextureRect(idleRects[0]);
    sprite.setOrigin(idleRects[0].width / 2.0f, idleRects[0].height - (PLAYER_HEIGHT / 2.0f / PLAYER_SCALE));
    sprite.setScale(PLAYER_SCALE, PLAYER_SCALE);
}

void Player::StopSound() {
    runSound.stop();
    fallSound.stop();
    brakeSound.stop();
}

void Player::Draw(sf::RenderWindow& window) {
    window.draw(sprite);
}