#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "../Map/Map.h"

struct Player {
    int hp{100};
    int maxHp{100};
    bool isFinished{false};
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
    Player();
    void HandleHorizontalMovement(float dt);
    void HandleVerticalMovement(float dt, bool jumpPressed);
    void ResolveCollisions(const Map& map, float dt);
    void Update(float dt);
    void Reset();
    void Draw(sf::RenderWindow& window);
};