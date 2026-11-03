#include "App.h"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <iostream>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Color.hpp>
#include <algorithm>

const std::string FONT_PATH = "/System/Library/Fonts/Helvetica.ttc";
constexpr unsigned int FRAMERATE = 60;
constexpr float ZOOM_LEVEL = 0.9f;
constexpr float TIME_STEP = 1.0f / 60.0f;
const std::string MENU_BG_PATH = "../assets/main_menu.jpg";
const std::string MENU_BUTTONS_PATH = "../assets/menu_buttons.png";
const sf::IntRect MENU_PLAY_IDLE(108, 212, 1050, 600);
const sf::IntRect MENU_PLAY_HOVER(1258, 195, 1050, 800);
const sf::IntRect MENU_EXIT_IDLE(108, 1060, 1050, 600);
const sf::IntRect MENU_EXIT_HOVER(1258, 1060, 1050, 800);
const sf::Vector2f MENU_BTN_SIZE(350.0f, 150.0f);
constexpr float MENU_CENTER_OFFSET_X = 175.0f;
constexpr float MENU_BTN_OFFSET_Y = 80.0f;
const sf::Color MENU_BG_COLOR(30, 30, 40);
const std::string PAUSE_BUTTONS_PATH = "../assets/pause_buttons.png";
const std::string PAUSE_PANEL_PATH = "../assets/pause_screen.png";
constexpr float PAUSE_PANEL_SCALE = 0.6f;
const sf::IntRect PAUSE_RESUME_IDLE(108, 212, 1050, 600);
const sf::IntRect PAUSE_RESUME_HOVER(1258, 195, 1050, 800);
const sf::IntRect PAUSE_MENU_IDLE(108, 1060, 1050, 600);
const sf::IntRect PAUSE_MENU_HOVER(1258, 1060, 1050, 800);
const sf::Vector2f PAUSE_BTN_SIZE(400.0f, 150.0f);
constexpr float PAUSE_CENTER_OFFSET_X = 200.0f;
constexpr float PAUSE_RESUME_OFFSET_Y = 110.0f;
constexpr float PAUSE_MENU_OFFSET_Y = 70.0f;
const sf::Color PAUSE_OVERLAY_COLOR(0, 0, 0, 160);


App::App() {
    if (!menuFont.loadFromFile(FONT_PATH)) {
        std::cout << "Failed to load system font" << std::endl;
    }
    window.setFramerateLimit(FRAMERATE);
    view.setSize(
        static_cast<float>(window.getSize().x),
        static_cast<float>(window.getSize().y)
    );
    view.zoom(ZOOM_LEVEL);
    view.setCenter(
        window.getSize().x / 2.0f,
        window.getSize().y / 2.0f
    );
}

void App::Run() {
    while (window.isOpen() && currentState != ScreenState::Exit) {
        switch (currentState) {
            case ScreenState::Menu:
                runMenu();
                break;
            case ScreenState::Game:
                runGame();
                break;
            case ScreenState::Pause:
                runPause();
                break;
            case ScreenState::GameOver:
                runGameOver();
                break;
            case ScreenState::GameWin:
                runGameWin();
                break;
            case ScreenState::Exit:
                runExit();
                break;
        }
    }
}

void App::runGame() {
    while (window.isOpen() && currentState == ScreenState::Game) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                currentState = ScreenState::Exit;
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                currentState = ScreenState::Pause;
            }
        }
        const float dt = TIME_STEP;
        player.Update(dt);
        player.ResolveCollisions(level.map, dt);
        hud.Update(player.hp);
        if (player.hp <= 0) {
            currentState = ScreenState::GameOver;
        }
        if (player.isFinished) {
            currentState = ScreenState::GameWin;
        }
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
            clampedX = std::clamp(targetCenter.x, halfWidth, mapWidth - halfWidth);
        } else {
            clampedX = mapWidth / 2.0f;
        }
        if (mapHeight >= view.getSize().y) {
            clampedY = std::clamp(targetCenter.y, halfHeight, mapHeight - halfHeight);
        } else {
            clampedY = mapHeight / 2.0f;
        }
        view.setCenter(clampedX, clampedY);
        window.clear(sf::Color::Black);
        window.setView(view);
        level.Draw(window);
        player.Draw(window);
        hud.Draw(window);
        window.display();
    }
}

void App::runMenu() {
    sf::Texture bgTexture;
    if (!bgTexture.loadFromFile(MENU_BG_PATH)) {
        std::cout << "Failed to load main_menu.jpg" << std::endl;
    }
    sf::Sprite bgSprite(bgTexture);
    bgSprite.setScale(
        static_cast<float>(window.getSize().x) / bgTexture.getSize().x,
        static_cast<float>(window.getSize().y) / bgTexture.getSize().y
    );
    sf::Texture buttonsSheet;
    if (!buttonsSheet.loadFromFile(MENU_BUTTONS_PATH)) {
        std::cout << "Failed to load menu_buttons.png" << std::endl;
    }
    sf::Text tutorialText(L"Управление:\n< ^ v > - Движение\nЛКМ - Атака\nESC - Пауза", menuFont, 30);
    tutorialText.setFillColor(sf::Color(255, 255, 255, 200));
    tutorialText.setPosition(30.0f, window.getSize().y - 150.0f);
    float centerX = window.getSize().x / 2.0f - MENU_CENTER_OFFSET_X;
    float centerY = window.getSize().y / 2.0f;
    sf::IntRect playIdleRect = MENU_PLAY_IDLE;
    sf::IntRect playHoverRect = MENU_PLAY_HOVER;
    sf::IntRect exitIdleRect = MENU_EXIT_IDLE;
    sf::IntRect exitHoverRect = MENU_EXIT_HOVER;
    Button playBtn(buttonsSheet, playIdleRect, playHoverRect, MENU_BTN_SIZE, {centerX, centerY - MENU_BTN_OFFSET_Y});
    Button exitBtn(buttonsSheet, exitIdleRect, exitHoverRect, MENU_BTN_SIZE, {centerX, centerY + MENU_BTN_OFFSET_Y});
    while (window.isOpen() && currentState == ScreenState::Menu) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) currentState = ScreenState::Exit;
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                currentState = ScreenState::Game;
            }
            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                if (playBtn.isClicked(window, event.mouseButton.x, event.mouseButton.y)) {
                    currentState = ScreenState::Game;
                }
                if (exitBtn.isClicked(window, event.mouseButton.x, event.mouseButton.y)) {
                    currentState = ScreenState::Exit;
                }
            }
        }
        playBtn.Update(window);
        exitBtn.Update(window);
        window.clear(MENU_BG_COLOR);
        window.setView(window.getDefaultView());
        window.draw(bgSprite);
        window.draw(tutorialText);
        playBtn.Draw(window);
        exitBtn.Draw(window);
        window.display();
    }
}

void App::runPause() {
    sf::Texture pauseButtonsSheet;
    if (!pauseButtonsSheet.loadFromFile(PAUSE_BUTTONS_PATH)) {
        std::cout << "Failed to load pause_buttons.png" << std::endl;
    }
    sf::Texture pausePanelTex;
    if (!pausePanelTex.loadFromFile(PAUSE_PANEL_PATH)) {
        std::cout << "Failed to load pause_panel.png" << std::endl;
    }
    sf::Sprite pausePanel(pausePanelTex);
    sf::FloatRect panelBounds = pausePanel.getLocalBounds();
    pausePanel.setOrigin(panelBounds.width / 2.0f, panelBounds.height / 2.0f);
    pausePanel.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f);
    pausePanel.setScale(PAUSE_PANEL_SCALE, PAUSE_PANEL_SCALE);
    sf::IntRect resumeIdleRect = PAUSE_RESUME_IDLE;
    sf::IntRect resumeHoverRect = PAUSE_RESUME_HOVER;
    sf::IntRect menuIdleRect = PAUSE_MENU_IDLE;
    sf::IntRect menuHoverRect = PAUSE_MENU_HOVER;
    float centerX = window.getSize().x / 2.0f - PAUSE_CENTER_OFFSET_X;
    float centerY = window.getSize().y / 2.0f;
    Button resumeBtn(pauseButtonsSheet, resumeIdleRect, resumeHoverRect, PAUSE_BTN_SIZE, {centerX, centerY - PAUSE_RESUME_OFFSET_Y});
    Button menuBtn(pauseButtonsSheet, menuIdleRect, menuHoverRect, PAUSE_BTN_SIZE, {centerX, centerY + PAUSE_MENU_OFFSET_Y});
    sf::RectangleShape darkOverlay(sf::Vector2f(window.getSize().x, window.getSize().y));
    darkOverlay.setFillColor(PAUSE_OVERLAY_COLOR);
    while (window.isOpen() && currentState == ScreenState::Pause) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) currentState = ScreenState::Exit;
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                currentState = ScreenState::Game;
            }
            if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
                if (resumeBtn.isClicked(window, event.mouseButton.x, event.mouseButton.y)) {
                    currentState = ScreenState::Game;
                }
                if (menuBtn.isClicked(window, event.mouseButton.x, event.mouseButton.y)) {
                    currentState = ScreenState::Menu;
                }
            }
        }
        resumeBtn.Update(window);
        menuBtn.Update(window);
        window.clear();
        window.setView(view);
        level.Draw(window);
        player.Draw(window);
        hud.Draw(window);
        window.setView(window.getDefaultView());
        window.draw(darkOverlay);
        window.draw(pausePanel);
        resumeBtn.Draw(window);
        menuBtn.Draw(window);
        window.display();
    }
}

void App::runGameOver() {
    sf::Texture loseTexture;
    if (!loseTexture.loadFromFile("../assets/gameOver_screen.jpg")) {
        std::cout << "Failed to load game_over_screen.jpg" << std::endl;
    }
    sf::Sprite loseSprite(loseTexture);
    loseSprite.setScale(
        static_cast<float>(window.getSize().x) / loseTexture.getSize().x,
        static_cast<float>(window.getSize().y) / loseTexture.getSize().y
    );
    while (window.isOpen() && currentState == ScreenState::GameOver) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                currentState = ScreenState::Exit;
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                player.Reset();
                currentState = ScreenState::Game;
            }
        }
        window.clear();
        window.setView(window.getDefaultView());
        window.draw(loseSprite);
        window.display();
    }
}

void App::runGameWin() {
    sf::Texture winTexture;
    if (!winTexture.loadFromFile("../assets/win_screen.jpg")) {
        std::cout << "Failed to load victory_screen.jpg" << std::endl;
    }
    sf::Sprite winSprite(winTexture);
    winSprite.setScale(
        static_cast<float>(window.getSize().x) / winTexture.getSize().x,
        static_cast<float>(window.getSize().y) / winTexture.getSize().y
    );
    while (window.isOpen() && currentState == ScreenState::GameWin) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                currentState = ScreenState::Exit;
            }
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
                player.Reset();
                currentState = ScreenState::Game;
            }
        }
        window.clear();
        window.setView(window.getDefaultView());
        window.draw(winSprite);
        window.display();
    }
}

void App::runExit() {
    window.close();
}