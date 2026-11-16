#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "../Map/Map.h"

struct Player {
    int hp{100};
    int maxHp{100};
    bool isFinished{false};
    sf::Vector2f spawnPosition{500.0f, 500.0f};
    sf::Sprite sprite;
    sf::Texture texture;
    sf::Vector2f velocity;
    sf::SoundBuffer runBuffer;
    sf::Sound runSound;
    bool isRunLoaded = false;
    sf::SoundBuffer fallBuffer;
    sf::Sound fallSound;
    bool isFallLoaded = false;
    sf::SoundBuffer brakeBuffer;
    sf::Sound brakeSound;
    bool isBrakeLoaded = false;
    bool isGrounded;
    bool touchingLeft;
    bool touchingRight;
    Player(sf::Vector2f startPos = {200.0f, 100.0f});
    void SetSpawnPosition(sf::Vector2f pos);
    void HandleHorizontalMovement(float dt);
    void HandleVerticalMovement(float dt, bool jumpPressed);
    void ResolveCollisions(const Map& map, float dt);
    bool IsNearVoice(const Map& map) const;
    bool TryPickupVoice(Map& map);
    void Update(float dt);
    void Reset();
    void Draw(sf::RenderWindow& window);
    std::vector<sf::IntRect> idleRects;
    std::vector<sf::IntRect> runRects;
    std::vector<sf::IntRect> jumpRects;
    std::vector<sf::IntRect> wallSlideRects;
    float animTimer = 0.0f;
    int currentFrame = 0;
    const std::vector<sf::IntRect>* lastActiveRects = nullptr;
};