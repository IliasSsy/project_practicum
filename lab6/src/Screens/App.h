#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "../Player/Player.h"
#include "../Levels/Level.h"
#include "../UI/Hud/Hud.h"
#include "../UI/Button/Button.h"

enum ScreenState : int {
    Menu,
    Game,
    Pause,
    GameOver,
    GameWin,
    Exit
};

struct App {
    sf::RenderWindow window{sf::VideoMode(2560, 1664), "017 Prison"};
    HUD hud;
    Player player;
    Level level;
    sf::Clock clock;
    sf::View view;
    sf::Font menuFont;
    sf::Clock levelClock;
    ScreenState currentState{ScreenState::Menu};

    bool showVoicePrompt = false;
    bool isEnterPressedForVoice = false;

    App();
    void Run();
    void RunMenu();
    void RunGame();
    void RunPause();
    void RunGameOver();
    void RunGameWin();
    void RunExit();

    sf::Texture LoadTextureSafe(const std::string& path);
    bool LoadMusicSafe(sf::Music& music, const std::string& path, float volume, bool loop = true);
    sf::Sound LoadSoundSafe(const std::string& path, float volume);
    sf::Sprite CreateScaledSprite(const sf::Texture& texture);

    void UpdateCamera();
    void DrawWorldClocks();
    void ProcessInput(Button* btn1 = nullptr, Button* btn2 = nullptr);
    void Update(float dt, sf::Text* voicePromptText = nullptr, sf::Sound* dataPadSound = nullptr, Button* btn1 = nullptr, Button* btn2 = nullptr);
    void Draw(sf::Sprite* bgOrEndSprite = nullptr, sf::Text* textObj = nullptr, sf::RectangleShape* overlay = nullptr, Button* btn1 = nullptr, Button* btn2 = nullptr);
};