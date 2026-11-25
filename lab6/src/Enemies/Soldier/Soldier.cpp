#include "Soldier.h"
#include <random>
#include <cmath>
#include <algorithm>

constexpr float BASE_SOLDIER_WIDTH = 32.0f;
constexpr float BASE_SOLDIER_HEIGHT = 90.0f;
constexpr float GRAVITY = 980.0f;
constexpr float EPSILON = 0.001f;

static bool IsSoldierTileSolid(TileType type) {
    return type != TileType::Empty &&
           type != TileType::Voice &&
           type != TileType::Clock &&
           type != TileType::Door;
}

static void ResolveSoldierCollisions(sf::Sprite& sprite, sf::Vector2f& velocity, bool& facingRight, float scale, bool isAlerted, const Map& map, float dt) {
    dt = std::min(dt, 0.05f);
    sf::Vector2f pos = sprite.getPosition();

    float width = BASE_SOLDIER_WIDTH * scale;
    float height = BASE_SOLDIER_HEIGHT * scale;

    pos.x += velocity.x * dt;
    float shrinkY = 4.0f;
    sf::FloatRect boxX(
        pos.x - width / 2.0f,
        pos.y - height + shrinkY,
        width,
        height - shrinkY * 2.0f
    );

    int startCol = std::max(0, static_cast<int>(std::floor(boxX.left / TILE_SIZE)));
    int endCol   = std::min(static_cast<int>(map.width) - 1, static_cast<int>(std::floor((boxX.left + boxX.width - EPSILON) / TILE_SIZE)));
    int startRow = std::max(0, static_cast<int>(std::floor(boxX.top / TILE_SIZE)));
    int endRow   = std::min(static_cast<int>(map.height) - 1, static_cast<int>(std::floor((boxX.top + boxX.height - EPSILON) / TILE_SIZE)));

    for (int row = startRow; row <= endRow; ++row) {
        for (int col = startCol; col <= endCol; ++col) {
            if (!IsSoldierTileSolid(map.grid[row * map.width + col])) continue;

            sf::FloatRect tile(col * TILE_SIZE, row * TILE_SIZE, TILE_SIZE, TILE_SIZE);
            if (!boxX.intersects(tile)) continue;

            if (velocity.x > 0.0f) {
                pos.x = tile.left - width / 2.0f - EPSILON;
                velocity.x = 0.0f;
                if (!isAlerted) {
                    facingRight = false;
                    sprite.setScale(-scale, scale);
                }
            } else if (velocity.x < 0.0f) {
                pos.x = tile.left + tile.width + width / 2.0f + EPSILON;
                velocity.x = 0.0f;
                if (!isAlerted) {
                    facingRight = true;
                    sprite.setScale(scale, scale);
                }
            }
            boxX.left = pos.x - width / 2.0f;
        }
    }

    velocity.y += GRAVITY * dt;
    pos.y += velocity.y * dt;

    float shrinkX = 4.0f;
    sf::FloatRect boxY(
        pos.x - width / 2.0f + shrinkX,
        pos.y - height,
        width - shrinkX * 2.0f,
        height
    );

    startCol = std::max(0, static_cast<int>(std::floor(boxY.left / TILE_SIZE)));
    endCol   = std::min(static_cast<int>(map.width) - 1, static_cast<int>(std::floor((boxY.left + boxY.width - EPSILON) / TILE_SIZE)));
    startRow = std::max(0, static_cast<int>(std::floor(boxY.top / TILE_SIZE)));
    endRow   = std::min(static_cast<int>(map.height) - 1, static_cast<int>(std::floor((boxY.top + boxY.height - EPSILON) / TILE_SIZE)));

    for (int row = startRow; row <= endRow; ++row) {
        for (int col = startCol; col <= endCol; ++col) {
            if (!IsSoldierTileSolid(map.grid[row * map.width + col])) continue;

            sf::FloatRect tile(col * TILE_SIZE, row * TILE_SIZE, TILE_SIZE, TILE_SIZE);
            if (!boxY.intersects(tile)) continue;

            if (velocity.y > 0.0f) {
                pos.y = tile.top - EPSILON;
                velocity.y = 0.0f;
            } else if (velocity.y < 0.0f) {
                pos.y = tile.top + tile.height + height + EPSILON;
                velocity.y = 0.0f;
            }
            boxY.top = pos.y - height;
        }
    }

    sprite.setPosition(pos);
}

void Soldier::Init(const sf::Texture& texture, sf::Vector2f startPos) {
    scale = 0.6f;
    InitBase(texture, startPos);
    static std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<int> magDist(6, 8);
    int spareMagazines = magDist(gen);

    ammo = maxAmmo;
    totalAmmo = spareMagazines * maxAmmo;

    idleRects = {
        {89, 52, 119, 180},
        {390, 52, 120, 181},
        {688, 52, 123, 188},
        {986, 44, 125, 188},
        {1287, 45, 120, 188},
        {1589, 44, 128, 128}
    };
    walkRects = {
        {84, 286, 114, 183},
        {378, 280, 125, 188},
        {686, 279, 115, 188},
        {982, 280, 128, 185},
        {1285, 285, 137, 189},
        {1576, 281, 121, 189},
        {1887, 278, 116, 189},
    };
    shootRects = {
        {0, 256, 128, 128},
        {128, 256, 128, 128}
    };
    surrenderRects = {
        {0, 384, 128, 128}
    };
    deadRects = {
        {0, 512, 128, 128}
    };

    UpdateAnimation(0.0f, idleRects, 0.15f, true);
}

void Soldier::Update(float dt, const Map& map, sf::Vector2f playerPos) {
    if (isDead) {
        velocity.x = 0.0f;
        ResolveSoldierCollisions(sprite, velocity, facingRight, scale, true, map, dt);
        UpdateAnimation(dt, deadRects, 0.12f, false);
        return;
    }
    if (isSurrendering) {
        velocity.x = 0.0f;
        ResolveSoldierCollisions(sprite, velocity, facingRight, scale, true, map, dt);
        UpdateAnimation(dt, surrenderRects, 0.15f, false);
        return;
    }

    float dx = playerPos.x - sprite.getPosition().x;
    float dy = playerPos.y - sprite.getPosition().y;
    float distance = std::sqrt(dx * dx + dy * dy);
    bool isPlayerInFront = (facingRight && dx >= 0.0f) || (!facingRight && dx < 0.0f);
    bool isAlerted = isPlayerInFront && distance <= longRange && std::abs(dy) < 100.0f;

    if (isAlerted) {
        UpdateFacing(playerPos.x);
        velocity.x = 0.0f;

        if (distance <= closeRange) {
            AddFear(25.0f * dt);
        }

        UpdateShooting(dt, distance);
    } else {
        isShooting = false;

        if (fear > 0.0f) {
            fear = std::max(0.0f, fear - 5.0f * dt);
        }

        float patrolSpeed = 80.0f;
        velocity.x = facingRight ? patrolSpeed : -patrolSpeed;
    }

    ResolveSoldierCollisions(sprite, velocity, facingRight, scale, isAlerted, map, dt);

    if (isShooting) {
        UpdateAnimation(dt, shootRects, 0.1f, true);
    } else if (std::abs(velocity.x) > 0.1f) {
        UpdateAnimation(dt, walkRects, 0.1f, true);
    } else {
        UpdateAnimation(dt, idleRects, 0.15f, true);
    }
}

void Soldier::UpdateShooting(float dt, float distanceToPlayer) {
    if (shootCooldown > 0.0f) {
        shootCooldown -= dt;
        return;
    }

    if (ammo <= 0) {
        if (totalAmmo > 0) {
            int bulletsToLoad = std::min(maxAmmo, totalAmmo);
            ammo = bulletsToLoad;
            totalAmmo -= bulletsToLoad;
            burstShotsLeft = 0;
            shootCooldown = 2.0f;
            isShooting = false;
        } else {
            AddFear(maxFear);
        }
        return;
    }

    if (burstShotsLeft <= 0) {
        if (distanceToPlayer <= closeRange) {
            burstShotsLeft = ammo;
        } else if (distanceToPlayer <= midRange) {
            burstShotsLeft = std::min(3, ammo);
        } else {
            burstShotsLeft = 1;
        }
    }

    isShooting = true;
    ammo--;
    burstShotsLeft--;

    if (ammo <= 0) {
        shootCooldown = 2.0f;
    } else if (burstShotsLeft > 0) {
        shootCooldown = 0.12f;
    } else {
        shootCooldown = (distanceToPlayer <= midRange) ? 1.0f : 1.5f;
        isShooting = false;
    }
}

void Soldier::TakeDamage(int damage) {
    if (isDead || isSurrendering) return;

    hp -= damage;
    if (hp <= 0) {
        hp = 0;
        isDead = true;
        isShooting = false;
        velocity.x = 0.0f;
        return;
    }

    AddFear(35.0f);
}

void Soldier::AddFear(float amount) {
    if (isDead || isSurrendering) return;

    fear += amount;
    if (fear >= maxFear) {
        fear = maxFear;
        isSurrendering = true;
        isShooting = false;
        velocity.x = 0.0f;
    }
}