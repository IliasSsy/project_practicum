#include "Level.h"

Level::Level() {
    sf::RectangleShape ground(sf::Vector2f(800.f, 50.f));
    ground.setFillColor(sf::Color::Green);
    ground.setPosition(0.f, 550.f);
    platform.push_back(ground);

    sf::RectangleShape leftWall(sf::Vector2f(20.f, 550.f));
    leftWall.setFillColor(sf::Color::Green);
    leftWall.setPosition(0.f, 0.f);
    platform.push_back(leftWall);

    sf::RectangleShape rightWall(sf::Vector2f(20.f, 550.f));
    rightWall.setFillColor(sf::Color::Green);
    rightWall.setPosition(780.f, 0.f);
    platform.push_back(rightWall);
}

void Level::Draw(sf::RenderWindow& window) {
    for (const sf::RectangleShape& shape : platform) {
        window.draw(shape);
    }
}