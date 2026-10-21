#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "../Map/Map.h"

struct Player {
    sf::Sprite sprite;
    sf::Texture texture;
    sf::Vector2f velocity;

    sf::SoundBuffer runBuffer;
    sf::Sound runSound;

    sf::SoundBuffer fallBuffer;
    sf::Sound fallSound;

    sf::SoundBuffer brakeBuffer;
    sf::Sound brakeSound;

    bool isGrounded;
    bool touchingLeft;
    bool touchingRight;

    Player();

    void HandleHorizontalMovement(float dt);
    void HandleVerticalMovement(float dt, bool jumpPressed);
    void ResolveCollisions(const Map& map, float dt);
    void Update(float dt);
    void Draw(sf::RenderWindow& window);
};