#include "App.h"
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Event.hpp>
#include <iostream>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Color.hpp>
#include <cmath>
#include <cstdio>

const std::string FONT_PATH = "/System/Library/Fonts/Helvetica.ttc";
const unsigned int FRAMERATE = 60;
const float ZOOM_LEVEL = 0.55f;
const float CAMERA_SMOOTH = 8.0f;
const float LIGHT_INNER_RADIUS = 110.0f;
const float LIGHT_OUTER_RADIUS = 320.0f;
const sf::Uint8 LIGHT_DARKNESS = 248;
const float TIME_STEP = 1.0f / 60.0f;

const std::string MENU_BG_PATH = "../assets/main_menu.jpg";
const std::string MENU_BUTTONS_PATH = "../assets/menu_buttons.png";
const sf::IntRect MENU_PLAY_IDLE(108, 212, 1050, 600);
const sf::IntRect MENU_PLAY_HOVER(1258, 195, 1050, 800);
const sf::IntRect MENU_EXIT_IDLE(108, 1060, 1050, 600);
const sf::IntRect MENU_EXIT_HOVER(1258, 1060, 1050, 800);
const sf::Vector2f MENU_BTN_SIZE(350.0f, 150.0f);
const float MENU_CENTER_OFFSET_X = 175.0f;
const float MENU_BTN_OFFSET_Y = 80.0f;
const sf::Color MENU_BG_COLOR(30, 30, 40);

const std::string PAUSE_BUTTONS_PATH = "../assets/pause_buttons.png";
const std::string PAUSE_PANEL_PATH = "../assets/pause_screen.png";
const float PAUSE_PANEL_SCALE = 0.6f;
const sf::IntRect PAUSE_RESUME_IDLE(108, 212, 1050, 600);
const sf::IntRect PAUSE_RESUME_HOVER(1258, 195, 1050, 800);
const sf::IntRect PAUSE_MENU_IDLE(108, 1060, 1050, 600);
const sf::IntRect PAUSE_MENU_HOVER(1258, 1060, 1050, 800);
const sf::Vector2f PAUSE_BTN_SIZE(400.0f, 150.0f);
const float PAUSE_CENTER_OFFSET_X = 200.0f;
const float PAUSE_RESUME_OFFSET_Y = 110.0f;
const float PAUSE_MENU_OFFSET_Y = 70.0f;
const sf::Color PAUSE_OVERLAY_COLOR(0, 0, 0, 160);

App::App() {
    level.Init();
    if (!menuFont.loadFromFile(FONT_PATH)) {
        std::cout << "Failed to load system font" << std::endl;
    }
    window.setFramerateLimit(FRAMERATE);
    view.setSize(
        static_cast<float>(window.getSize().x),
        static_cast<float>(window.getSize().y)
    );
    view.zoom(ZOOM_LEVEL);
    view.setCenter(player.sprite.getPosition());
}

void App::Run() {
    while (window.isOpen() && currentState != ScreenState::Exit) {
        switch (currentState) {
            case ScreenState::Menu:
                RunMenu();
                break;
            case ScreenState::Game:
                RunGame();
                break;
            case ScreenState::Pause:
                RunPause();
                break;
            case ScreenState::GameOver:
                RunGameOver();
                break;
            case ScreenState::GameWin:
                RunGameWin();
                break;
            case ScreenState::Exit:
                RunExit();
                break;
        }
    }
}

void App::RunGame() {
    levelClock.restart();
    view.setCenter(player.sprite.getPosition());
    sf::Music gameMusic;
    LoadMusicSafe(gameMusic, "../assets/music/game.mp3", 15.0f);
    sf::Sound dataPadSound = LoadSoundSafe("../assets/music/data-pad.mp3", 50.0f);
    sf::Text voicePromptText;
    voicePromptText.setFont(menuFont);
    voicePromptText.setCharacterSize(18);
    voicePromptText.setFillColor(sf::Color::Green);
    voicePromptText.setString("Press Enter to Read");
    while (window.isOpen() && currentState == ScreenState::Game) {
        ProcessInput();
        Update(TIME_STEP, &voicePromptText, &dataPadSound);
        Draw(nullptr, &voicePromptText);
    }
}

void App::RunMenu() {
    sf::Texture bgTexture = LoadTextureSafe(MENU_BG_PATH);
    sf::Sprite bgSprite = CreateScaledSprite(bgTexture);
    sf::Music menuMusic;
    LoadMusicSafe(menuMusic, "../assets/music/main_music.mp3", 100.0f);
    sf::Texture buttonsSheet = LoadTextureSafe(MENU_BUTTONS_PATH);
    sf::Text tutorialText(L"Управление:\n< ^ v > - Движение\nЛКМ - Атака\nESC - Пауза", menuFont, 30);
    tutorialText.setFillColor(sf::Color(255, 255, 255, 200));
    tutorialText.setPosition(30.0f, window.getSize().y - 150.0f);
    float centerX = window.getSize().x / 2.0f - MENU_CENTER_OFFSET_X;
    float centerY = window.getSize().y / 2.0f;
    Button playBtn(buttonsSheet, MENU_PLAY_IDLE, MENU_PLAY_HOVER, MENU_BTN_SIZE, {centerX, centerY - MENU_BTN_OFFSET_Y});
    Button exitBtn(buttonsSheet, MENU_EXIT_IDLE, MENU_EXIT_HOVER, MENU_BTN_SIZE, {centerX, centerY + MENU_BTN_OFFSET_Y});
    while (window.isOpen() && currentState == ScreenState::Menu) {
        ProcessInput(&playBtn, &exitBtn);
        Update(TIME_STEP, nullptr, nullptr, &playBtn, &exitBtn);
        Draw(&bgSprite, &tutorialText, nullptr, &playBtn, &exitBtn);
    }
}

void App::RunPause() {
    sf::Texture pauseButtonsSheet = LoadTextureSafe(PAUSE_BUTTONS_PATH);
    sf::Texture pausePanelTex = LoadTextureSafe(PAUSE_PANEL_PATH);
    player.StopSound();
    sf::Music pauseMusic;
    LoadMusicSafe(pauseMusic, "../assets/music/main_music.mp3", 15.0f);
    sf::Sprite pausePanel(pausePanelTex);
    sf::FloatRect panelBounds = pausePanel.getLocalBounds();
    pausePanel.setOrigin(panelBounds.width / 2.0f, panelBounds.height / 2.0f);
    pausePanel.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f);
    pausePanel.setScale(PAUSE_PANEL_SCALE, PAUSE_PANEL_SCALE);
    float centerX = window.getSize().x / 2.0f - PAUSE_CENTER_OFFSET_X;
    float centerY = window.getSize().y / 2.0f;
    Button resumeBtn(pauseButtonsSheet, PAUSE_RESUME_IDLE, PAUSE_RESUME_HOVER, PAUSE_BTN_SIZE, {centerX, centerY - PAUSE_RESUME_OFFSET_Y});
    Button menuBtn(pauseButtonsSheet, PAUSE_MENU_IDLE, PAUSE_MENU_HOVER, PAUSE_BTN_SIZE, {centerX, centerY + PAUSE_MENU_OFFSET_Y});
    sf::RectangleShape darkOverlay(
        sf::Vector2f(window.getSize().x, window.getSize().y)
    );
    darkOverlay.setFillColor(PAUSE_OVERLAY_COLOR);
    while (window.isOpen() && currentState == ScreenState::Pause) {
        ProcessInput(&resumeBtn, &menuBtn);
        Update(TIME_STEP, nullptr, nullptr, &resumeBtn, &menuBtn);
        Draw(&pausePanel, nullptr, &darkOverlay, &resumeBtn, &menuBtn);
    }
}

void App::RunGameOver() {
    sf::Texture loseTexture = LoadTextureSafe("../assets/gameOver_screen.jpg");
    sf::Sprite loseSprite = CreateScaledSprite(loseTexture);
    sf::Music loseMusic;
    LoadMusicSafe(loseMusic, "../assets/music/death.mp3", 50.0f, false);
    while (window.isOpen() && currentState == ScreenState::GameOver) {
        ProcessInput();
        Update(TIME_STEP);
        Draw(&loseSprite);
    }
}

void App::RunGameWin() {
    sf::Texture winTexture = LoadTextureSafe("../assets/win_screen.jpg");
    sf::Sprite winSprite = CreateScaledSprite(winTexture);
    sf::Music winMusic;
    LoadMusicSafe(winMusic, "../assets/music/win.mp3", 50.0f, false);
    while (window.isOpen() && currentState == ScreenState::GameWin) {
        ProcessInput();
        Update(TIME_STEP);
        Draw(&winSprite);
    }
}

void App::RunExit() {
    window.close();
}

void App::ProcessInput(Button* btn1, Button* btn2) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            currentState = ScreenState::Exit;
            return;
        }
        if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code) {
                case sf::Keyboard::Escape:
                    if (currentState == ScreenState::Game) currentState = ScreenState::Pause;
                    else if (currentState == ScreenState::Pause) currentState = ScreenState::Game;
                    break;
                case sf::Keyboard::Enter:
                    if (currentState == ScreenState::Menu) currentState = ScreenState::Game;
                    else if (currentState == ScreenState::GameOver || currentState == ScreenState::GameWin) {
                        player.Reset();
                        currentState = ScreenState::Game;
                    }
                    break;
                default:
                    break;
            }
        }
        if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
            if (btn1 && btn1->isClicked(window, event.mouseButton.x, event.mouseButton.y) && (currentState == ScreenState::Menu || currentState == ScreenState::Pause)) {
                currentState = ScreenState::Game;
            }
            if (btn2 && btn2->isClicked(window, event.mouseButton.x, event.mouseButton.y)) {
                if (currentState == ScreenState::Menu) currentState = ScreenState::Exit;
                else if (currentState == ScreenState::Pause) currentState = ScreenState::Menu;
            }
        }
    }
    if (currentState == ScreenState::Game)
        isEnterPressedForVoice = sf::Keyboard::isKeyPressed(sf::Keyboard::Enter);
}

void App::Update(float dt, sf::Text* voicePromptText, sf::Sound* dataPadSound, Button* btn1, Button* btn2) {
    switch (currentState) {
        case ScreenState::Menu:
        case ScreenState::Pause:
            if (btn1) btn1->Update(window);
            if (btn2) btn2->Update(window);
            break;
        case ScreenState::Game:
            player.Update(dt);
            player.ResolveCollisions(level.map, dt);
            level.Update(dt, player.sprite.getPosition());
            if (player.hp <= 0) {
                currentState = ScreenState::GameOver;
                break;
            }
            if (player.isFinished) {
                currentState = ScreenState::GameWin;
                break;
            }
            hud.Update(player.hp);
            UpdateCamera();
            showVoicePrompt = player.IsNearVoice(level.map);
            if (showVoicePrompt && voicePromptText) {
                sf::Vector2f pPos = player.sprite.getPosition();
                voicePromptText->setPosition(pPos.x - 45.0f, pPos.y - 75.0f);
                if (isEnterPressedForVoice &&
                    player.TryPickupVoice(level.map)) {
                    std::cout << "Служебный рапорт успешно прочитан!" << std::endl;
                    if (dataPadSound) {
                        dataPadSound->play();
                    }
                }
            }
            break;
        case ScreenState::GameOver:
        case ScreenState::GameWin:
        case ScreenState::Exit:
            break;
    }
}

void App::Draw(sf::Sprite* bgOrEndSprite, sf::Text* textObj, sf::RectangleShape* overlay, Button* btn1, Button* btn2) {
    if (currentState == ScreenState::Exit) return;
    window.clear(currentState == ScreenState::Menu ? MENU_BG_COLOR : sf::Color::Black);
    window.setView(currentState == ScreenState::Game || currentState == ScreenState::Pause ? view : window.getDefaultView());
    switch (currentState) {
        case ScreenState::Menu:
            if (bgOrEndSprite) window.draw(*bgOrEndSprite);
            if (textObj) window.draw(*textObj);
            break;
        case ScreenState::Game:
        case ScreenState::Pause:
            level.Draw(window);
            player.Draw(window);
            if (currentState == ScreenState::Game) {
                DrawWorldClocks();
            }
            DrawPlayerLight();
            if (currentState == ScreenState::Game && showVoicePrompt && textObj) {
                window.draw(*textObj);
            }
            hud.Draw(window);
            break;
        case ScreenState::GameOver:
        case ScreenState::GameWin:
            if (bgOrEndSprite) window.draw(*bgOrEndSprite);
            break;
        default:
            break;
    }
    if (currentState == ScreenState::Pause) {
        window.setView(window.getDefaultView());
        if (overlay) window.draw(*overlay);
        if (bgOrEndSprite) window.draw(*bgOrEndSprite);
    }
    if (currentState == ScreenState::Menu || currentState == ScreenState::Pause) {
        if (btn1) btn1->Draw(window);
        if (btn2) btn2->Draw(window);
    }
    window.display();
}

void App::UpdateCamera() {
    sf::Vector2f targetCenter = player.sprite.getPosition();
    sf::Vector2f currentCenter = view.getCenter();
    sf::Vector2f newCenter = currentCenter + (targetCenter - currentCenter) * CAMERA_SMOOTH * TIME_STEP;
    view.setCenter(newCenter);
}

void App::DrawPlayerLight() {
    sf::Vector2f center = player.sprite.getPosition();
    const int segments = 64;
    const float pi = 3.14159265f;
    const float maxExtent = 5000.0f;
    sf::VertexArray lightGradient(sf::TriangleStrip, (segments + 1) * 2);
    for (int i = 0; i <= segments; ++i) {
        float angle = (static_cast<float>(i) / segments) * 2.0f * pi;
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);
        lightGradient[i * 2].position = {center.x + cosA * LIGHT_INNER_RADIUS, center.y + sinA * LIGHT_INNER_RADIUS};
        lightGradient[i * 2].color = sf::Color(0, 0, 0, 0);
        lightGradient[i * 2 + 1].position = {center.x + cosA * LIGHT_OUTER_RADIUS, center.y + sinA * LIGHT_OUTER_RADIUS};
        lightGradient[i * 2 + 1].color = sf::Color(0, 0, 0, LIGHT_DARKNESS);
    }
    sf::VertexArray outerDarkness(sf::TriangleStrip, (segments + 1) * 2);
    for (int i = 0; i <= segments; ++i) {
        float angle = (static_cast<float>(i) / segments) * 2.0f * pi;
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);
        outerDarkness[i * 2].position = {center.x + cosA * LIGHT_OUTER_RADIUS, center.y + sinA * LIGHT_OUTER_RADIUS};
        outerDarkness[i * 2].color = sf::Color(0, 0, 0, LIGHT_DARKNESS);
        outerDarkness[i * 2 + 1].position = {center.x + cosA * maxExtent, center.y + sinA * maxExtent};
        outerDarkness[i * 2 + 1].color = sf::Color(0, 0, 0, LIGHT_DARKNESS);
    }

    window.draw(lightGradient);
    window.draw(outerDarkness);
}

void App::DrawWorldClocks() {
    float elapsedTime = levelClock.getElapsedTime().asSeconds();
    int totalSeconds = static_cast<int>(elapsedTime);
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;
    char timeBuffer[16];
    if (minutes >= 100) {
        std::snprintf(timeBuffer, sizeof(timeBuffer), "99:99");
    } else {
        std::snprintf(timeBuffer, sizeof(timeBuffer), "%02d:%02d", minutes, seconds);
    }
    sf::Text worldClockText;
    worldClockText.setFont(menuFont);
    worldClockText.setCharacterSize(10);
    worldClockText.setFillColor(sf::Color(255, 50, 40, 230));
    worldClockText.setString(timeBuffer);
    for (const auto& clockPos : level.map.clockPositions) {
        float screenX = clockPos.x * TILE_SIZE + 11.f;
        float screenY = clockPos.y * TILE_SIZE + 15.f;
        worldClockText.setPosition(screenX, screenY);
        window.draw(worldClockText);
    }
}

sf::Texture App::LoadTextureSafe(const std::string& path) {
    sf::Texture texture;
    if (!texture.loadFromFile(path)) {
        std::cout << "ERROR: Failed to load texture: " << path << std::endl;
    }
    return texture;
}

bool App::LoadMusicSafe(sf::Music& music, const std::string& path, float volume, bool loop) {
    if (music.openFromFile(path)) {
        music.setLoop(loop);
        music.setVolume(volume);
        music.play();
        return true;
    }
    std::cout << "ERROR: Failed to load music: " << path << std::endl;
    return false;
}

sf::Sound App::LoadSoundSafe(const std::string& path, float volume) {
    sf::Sound sound;
    static sf::SoundBuffer buffer;
    if (buffer.loadFromFile(path)) {
        sound.setBuffer(buffer);
        sound.setVolume(volume);
    } else {
        std::cout << "ERROR: Failed to load sound: " << path << std::endl;
    }
    return sound;
}

sf::Sprite App::CreateScaledSprite(const sf::Texture& texture) {
    sf::Sprite sprite(texture);
    if (texture.getSize().x == 0 || texture.getSize().y == 0) {
        return sprite;
    }
    sprite.setScale(
        static_cast<float>(window.getSize().x) / texture.getSize().x,
        static_cast<float>(window.getSize().y) / texture.getSize().y
    );
    return sprite;
}