#include "Hud.h"
#include <iostream>

HUD::HUD() {
    if (!font.loadFromFile("/System/Library/Fonts/Helvetica.ttc")) {
        std::cout << "Failed to load system font" << std::endl;
    }
    hpText.setFont(font);
    hpText.setCharacterSize(40);
    hpText.setFillColor(sf::Color::Red);
    hpText.setPosition(20.0f, 20.0f);
}

void HUD::Update(int playerHp) {
    hpText.setString(std::to_string(playerHp));
}

void HUD::Draw(sf::RenderWindow& window) {
    uiView.setSize(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y));
    uiView.setCenter(window.getSize().x / 2.0f, window.getSize().y / 2.0f);
    sf::View previousView = window.getView();
    window.setView(uiView);
    window.draw(hpText);
    window.setView(previousView);
}