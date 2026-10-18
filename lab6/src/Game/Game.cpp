#include <algorithm>
#include "Game.h"
#include "../Player/Player.h"

Game::Game() {
    window.setFramerateLimit(60);
    view.setSize(
        static_cast<float>(window.getSize().x),
        static_cast<float>(window.getSize().y)
    );
    view.zoom(0.9f);
    view.setCenter(
        window.getSize().x / 2.0f,
        window.getSize().y / 2.0f
    );
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
        player.Update(dt);
        player.ResolveCollisions(level.map, dt);
        sf::FloatRect playerBounds = player.sprite.getGlobalBounds();
        sf::Vector2f targetCenter(
            playerBounds.left + playerBounds.width / 2.0f,
            playerBounds.top + playerBounds.height / 2.0f
        );
        float halfWidth = view.getSize().x / 2.0f;
        float halfHeight = view.getSize().y / 2.0f;
        float mapWidth = level.map.width * TILE_SIZE;
        float mapHeight = level.map.height * TILE_SIZE;
        float clampedX;
        float clampedY;
        if (mapWidth >= view.getSize().x) {
            clampedX = std::clamp(
                targetCenter.x,
                halfWidth,
                mapWidth - halfWidth
            );
        } else {
            clampedX = mapWidth / 2.0f;
        }
        if (mapHeight >= view.getSize().y) {
            clampedY = std::clamp(
                targetCenter.y,
                halfHeight,
                mapHeight - halfHeight
            );
        } else {
            clampedY = mapHeight / 2.0f;
        }
        view.setCenter(clampedX, clampedY);
        window.clear(sf::Color::Black);
        window.setView(view);
        level.Draw(window);
        player.Draw(window);
        window.display();
    }
}
