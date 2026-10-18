#include "iostream"
#include "Game.h"
#include "../player/Player.h"

Game::Game() {
    window.setFramerateLimit(60);
}

void Game::Run() {
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        const float dt = 1.0f / 60.0f;

        player.Update(dt, player.touchingLeft, player.touchingRight);

        player.ResolveCollisions(level.platform);

        window.clear(sf::Color::Black);
        level.Draw(window);
        player.Draw(window);
        window.display();
    }
}