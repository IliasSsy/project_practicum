#pragma once
#include <SFML/Graphics.hpp>
#include "../Player/Player.h"
#include "../Levels/Level.h"
#include "../UI/Hud/Hud.h"
#include "../UI/Button/Button.h"

enum class ScreenState {
    Menu,
    Game,
    Pause,
    GameOver,
    GameWin,
    Exit
};

struct App {
    sf::RenderWindow window{sf::VideoMode(2560, 1664), "007 Prison"};
    HUD hud;
    Player player;
    Level level;
    sf::Clock clock;
    sf::View view;
    sf::Font menuFont;

    ScreenState currentState{ScreenState::Menu};

    App();
    void Run();

    void runMenu();
    void runGame();
    void runPause();
    void runGameOver();
    void runGameWin();
    void runExit();
};