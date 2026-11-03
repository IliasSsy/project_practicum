#pragma once
#include <SFML/Graphics.hpp>

struct Button {
    sf::Sprite sprite;
    sf::IntRect idleRect;
    sf::IntRect hoverRect;
    sf::Vector2f baseScale;

    Button(const sf::Texture& texture, sf::IntRect idle, sf::IntRect hover, sf::Vector2f size, sf::Vector2f position) {
        idleRect = idle;
        hoverRect = hover;

        sprite.setTexture(texture);
        sprite.setTextureRect(idleRect);

        baseScale.x = size.x / static_cast<float>(idleRect.width);
        baseScale.y = size.y / static_cast<float>(idleRect.height);
        sprite.setScale(baseScale);

        sprite.setOrigin(idleRect.width / 2.0f, idleRect.height / 2.0f);
        sprite.setPosition(position.x + size.x / 2.0f, position.y + size.y / 2.0f);
    }

    bool isMouseOver(sf::RenderWindow& window) {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        sf::FloatRect bounds = sprite.getGlobalBounds();

        bounds.left += 10.f; bounds.top += 10.f;
        bounds.width -= 20.f; bounds.height -= 20.f;

        return bounds.contains(mousePos);
    }

    bool isClicked(sf::RenderWindow& window, int eventX, int eventY) {
        sf::Vector2f clickPos = window.mapPixelToCoords(sf::Vector2i(eventX, eventY));
        sf::FloatRect bounds = sprite.getGlobalBounds();

        bounds.left += 10.f; bounds.top += 10.f;
        bounds.width -= 20.f; bounds.height -= 20.f;

        return bounds.contains(clickPos);
    }

    void Update(sf::RenderWindow& window) {
        if (isMouseOver(window)) {
            sprite.setTextureRect(hoverRect);
            sprite.setScale(baseScale.x * 1.05f, baseScale.y * 1.05f);
        } else {
            sprite.setTextureRect(idleRect);
            sprite.setScale(baseScale);
        }
    }

    void Draw(sf::RenderWindow& window) {
        window.draw(sprite);
    }
};