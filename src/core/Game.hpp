#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "GameState.hpp"
#include "CameraController.hpp"
#include "UI/MainMenu.h"
#include "UI/LevelMenu.h"
#include "UI/PauseMenu.h"
#include "UI/GameOverScreen.h"
#include "UI/HUD.h"
#include "Levels/LevelManager.hpp"

class Game {
public:
    Game();
    void run();

private:
    void processEvents();
    void update(float dt);
    void render();
    void handlePlayingInput();
    void handlePausedInput();
    void handleGameOverInput();
    void configureCameraForCurrentLevel();
    Level* getCurrentLevel();
    const Level* getCurrentLevel() const;
    void startLevel(int levelId);
    
    void changeState(GameState newState);
    void updateAudioForState();
    bool loadAudioAssets();
    
private:
    // Window & Views
    sf::RenderWindow window;
    sf::View worldView;
    sf::View uiView;

    // Camera
    CameraController camera;
    GameState currentState;

    // Level
    LevelManager levelManager;

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

    // Audio for the game,, there is no time lift to implement a more structured audio system.
    sf::Music menuMusic;
    sf::Music levelMusic;
    sf::SoundBuffer healingBuffer;
    sf::SoundBuffer hurtBuffer;
    sf::Sound healingSound;
    sf::Sound hurtSound;
    unsigned int lastHealingEventCount{0};
    unsigned int lastHurtEventCount{0};
};

#endif // GAME_HPP
