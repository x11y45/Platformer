#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include "GameState.hpp"
#include "CameraController.hpp"
#include "UI/MainMenu.h"
#include "UI/LevelMenu.h"
#include "UI/PauseMenu.h"
#include "UI/GameOverScreen.h"
#include "UI/HUD.h"
#include "Levels/map.h"

class Game {
public:
    Game();
    void run();

private:
    void processEvents();
    void update(float dt);
    void render();
    // Handle input.
    void handleMainMenuInput(sf::Vector2f worldMousePos);
    void handlePlayingInput();
    void handlePausedInput();
    void handleGameOverInput();
    
    void changeState(GameState newState);
    
private:
    // Window & Views
    sf::RenderWindow window;
    sf::View worldView;
    sf::View uiView;

    // Camera
    CameraController camera;
    GameState currentState;

    // gmae state
    MainMenu mainMenu;
    PauseMenu pauseMenu;
    LevelMenu levelMenu;
    GameOverScreen gameOverScreen;
    HUD hud;
    
    // Lives system
    int playerLives;
    int maxLives;
    
    // FPS tracking
    sf::Clock deltaClock;
};

#endif // GAME_HPP
