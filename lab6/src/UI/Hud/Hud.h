#pragma once
#include <SFML/Graphics.hpp>

struct HUD {
    sf::Font font;
    sf::Text hpText;
    sf::View uiView;

    HUD();
    void Update(int playerHp);
    void Draw(sf::RenderWindow& window);
};