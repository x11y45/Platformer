#ifndef GAME_HPP
#define GAME_HPP

#include <SFML/Graphics.hpp>
#include "GameState.hpp"
#include "CameraController.hpp"

class Game {
public:
    Game();
    
    void run();

private:
    void processEvents();
    void update(float dt);
    void render();
    // init game states
    void initMainMenu();
    void initPlaying();
    void initPaused();
    void intiLevelMenu();
    void initGameOver();
    void initButtons();
    // Handle input.
    void handleMainMenuInput(sf::Vector2f worldMousePos);
    void handlePlayingInput();
    void handlePausedInput();
    void handleGameOverInput();
    //Handle render.
    void renderMainMenu();
    void renderPlaying();
    void renderPause();
    void renderLevelMenu();
    void renderGameOver();
    // Update
    void updateMainMenu(float dt);
    void updatePlaying(float dt);
    void updatePaused(float dt);
    void updateLevelMenu(float dt);
    void updateGameOver(float dt);
    
    void changeState(GameState newState);
    
private:
    // Window & Views
    sf::RenderWindow window;
    sf::View worldView;
    sf::View uiView;

    // Camera
    CameraController camera;
    GameState currentState;

    // Main menu
    sf::Texture background;
    sf::Texture middleGround;
    sf::Sprite backgroundSprite;
    sf::Sprite middleGroundSpriteA;
    sf::Sprite middleGroundSpriteB;

    //Buttons
    sf::Texture menuButtonTexture;
    sf::Texture playButtonTexture;
    sf::Texture levelButtonTexture;
    sf::Texture gameOverTexture;
    sf::Texture pauseButtonTexture;

    sf::Sprite MenuButtonSprite;
    sf::Sprite PlayButtonSprite;
    sf::Sprite LevelButtonSprite;
    sf::Sprite GameOverSprite;
    sf::Sprite PauseButtonSprite;
    
    // Lives system
    int playerLives;
    int maxLives;
    
    // FPS tracking
    sf::Clock deltaClock;
};

#endif // GAME_HPP
