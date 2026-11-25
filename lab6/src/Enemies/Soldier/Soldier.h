#pragma once
#include "../Enemy.h"
#include "../Map/Map.h"

struct Soldier : Enemy {
    int hp = 100;
    int maxHp = 400;
    float fear = 0.0f;
    float maxFear = 100.0f;
    bool isSurrendering = false;
    bool isShooting = false;
    bool isGrounded = false;
    int ammo = 12;
    int maxAmmo = 12;
    int totalAmmo = 0;
    int burstShotsLeft = 0;
    float shootCooldown = 0.0f;
    float longRange = 450.0f;
    float midRange = 280.0f;
    float closeRange = 140.0f;
    std::vector<sf::IntRect> idleRects;
    std::vector<sf::IntRect> walkRects;
    std::vector<sf::IntRect> shootRects;
    std::vector<sf::IntRect> surrenderRects;
    std::vector<sf::IntRect> deadRects;
    void Init(const sf::Texture& texture, sf::Vector2f startPos);
    void Update(float dt, const Map& map, sf::Vector2f playerPos);
    void UpdateShooting(float dt, float distanceToPlayer);
    void TakeDamage(int damage);
    void AddFear(float amount);
    bool CanBeHit() const { return !isDead && !isSurrendering; }
};