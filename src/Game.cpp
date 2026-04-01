#include "Game.hpp"

Game::Game()
    : window(sf::VideoMode(800, 600), "GothicVania Platformer")
    , worldView(sf::FloatRect(0.f, 0.f, 800.f, 600.f))
    , uiView(sf::FloatRect(0.f, 0.f, 800.f, 600.f))
    , camera(800.f, 600.f)
    , currentState(GameState::MAIN_MENU)
    , playerLives(3)
    , maxLives(3)
{
    window.setFramerateLimit(60);
    worldView = camera.getView();
    initMainMenu();
    initGameOver();
    intiLevelMenu();
    initPlaying();
    initPaused();
    initButtons();
}

void Game::run() {
    while (window.isOpen()) {
        float dt = deltaClock.restart().asSeconds();
        processEvents();
        update(dt);
        render();
    }
}

void Game::processEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f worldMousePos = window.mapPixelToCoords(mousePos, uiView);

        if (event.type == sf::Event::MouseButtonPressed) {
            switch (currentState) {
                case GameState::MAIN_MENU:
                    handleMainMenuInput(worldMousePos);
                    break;
                case GameState::PLAYING:
                    handlePlayingInput();
                    break;
                case GameState::PAUSED:
                    handlePausedInput();
                    break;
                case GameState::GAME_OVER:
                    handleGameOverInput();
                    break;
                default:
                    break;
            }
        }
    }
}

void Game::update(float dt) {
    switch (currentState) {
        case GameState::MAIN_MENU:
            // TODO: Update menu animations
            updateMainMenu(dt);
            break;

        case GameState::PLAYING:
            // TODO: Update level, entities, collision
            camera.update(dt);
            worldView = camera.getView();
            break;
            
        case GameState::PAUSED:
            // No updates while paused
            break;
            
        case GameState::LEVEL_MENU:
            // TODO: Update level menu animation.
            break;
            
        case GameState::GAME_OVER:
            // TODO: Update game over screen animations
            break;
    }
}

void Game::render() {
    window.clear(sf::Color(20, 20, 35));
    
    switch (currentState) {
        case GameState::MAIN_MENU:
            window.setView(uiView);
            // TODO: Render main menu
            renderMainMenu();
            break;
            
        case GameState::PLAYING:
            // Render world
            window.setView(worldView);
            // TODO: Render level and entities
            
            // Render UI
            window.setView(uiView);
            // TODO: Render HUD (health, lives)
            break;
            
        case GameState::PAUSED:
            // Render world (frozen)
            window.setView(worldView);
            // TODO: Render level and entities
            
            // Render pause menu
            window.setView(uiView);
            // TODO: Render pause overlay
            break;
            
        case GameState::LEVEL_MENU:
            window.setView(uiView);
            break;
            
        case GameState::GAME_OVER:
            window.setView(uiView);
            // TODO: Render game over screen
            break;
    }
    window.display();
}
// input handling implementations
void Game::handleMainMenuInput(const sf::Vector2f worldMousePos) {
    // TODO: Menu navigation
    if (MenuButtonSprite.getGlobalBounds().contains(worldMousePos)) {
        changeState(GameState::MAIN_MENU);
    } else if (PlayButtonSprite.getGlobalBounds().contains(worldMousePos)) {
        changeState(GameState::PLAYING);
    } else if (LevelButtonSprite.getGlobalBounds().contains(worldMousePos)) {
        changeState(GameState::LEVEL_MENU);
    }
}
void Game::handlePlayingInput() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
        changeState(GameState::PAUSED);
    }
}
void Game::handlePausedInput() {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
        changeState(GameState::PLAYING);
    }
    // TODO: Handle resume/quit options
}
void Game::handleGameOverInput() {
    // TODO: Handle retry/quit options
}

void Game::changeState(GameState newState) {
    currentState = newState;
}
// render function implementation
void Game::renderMainMenu() {
    // TODO:
    window.draw(backgroundSprite);
    window.draw(middleGroundSpriteA);
    window.draw(middleGroundSpriteB);
    window.draw(MenuButtonSprite);
    window.draw(PlayButtonSprite);
    window.draw(LevelButtonSprite);
}
void Game::renderPlaying() {
    // TODO:
}
void Game::renderPause() {
    // TODO:
}
void Game::renderGameOver() {
    // TODO:
}
void Game::renderLevelMenu() {
}
//init game states
void Game::initGameOver() {

}
void Game::intiLevelMenu() {

}
void Game::initMainMenu() {
    background.loadFromFile("assets/background.png");
    middleGround.loadFromFile("assets/middleground.png");

    backgroundSprite.setTexture(background);
    backgroundSprite.setPosition(0.f,0.f);
    backgroundSprite.setScale(600.f/288,600.f/288);
    middleGroundSpriteA.setTexture(middleGround);
    middleGroundSpriteA.setPosition(0.f,0.f);
    middleGroundSpriteA.setScale(600.f/288,600.f/288);
    middleGroundSpriteB.setTexture(middleGround);
    middleGroundSpriteB.setPosition(0.f + middleGroundSpriteA.getGlobalBounds().width,0.f);
    middleGroundSpriteB.setScale(600.f/288,600.f/288);

}
void Game::initPaused() {

}
void Game::initPlaying() {

}
void Game::initButtons() {
    menuButtonTexture.loadFromFile("assets/menu_button.png");
    MenuButtonSprite.setTexture(menuButtonTexture);
    MenuButtonSprite.setOrigin(MenuButtonSprite.getGlobalBounds().width / 2.f, MenuButtonSprite.getGlobalBounds().height / 2.f);
    MenuButtonSprite.setScale(5.f, 5.f);
    MenuButtonSprite.setPosition(400.f, 200.f);

    playButtonTexture.loadFromFile("assets/play_button.png");
    PlayButtonSprite.setTexture(playButtonTexture);
    PlayButtonSprite.setOrigin(PlayButtonSprite.getGlobalBounds().width / 2.f, PlayButtonSprite.getGlobalBounds().height / 2.f);
    PlayButtonSprite.setScale(3.f, 3.f);
    PlayButtonSprite.setPosition(400.f, 300.f);

    levelButtonTexture.loadFromFile("assets/level_button.png");
    LevelButtonSprite.setTexture(levelButtonTexture);
    LevelButtonSprite.setOrigin(LevelButtonSprite.getGlobalBounds().width / 2.f, LevelButtonSprite.getGlobalBounds().height / 2.f);
    LevelButtonSprite.setScale(3.f, 3.f);
    LevelButtonSprite.setPosition(400.f, 400.f);
}
// Update function implementations
void Game::updateMainMenu(float dt) {
    float movement = 100.f * dt;
    const float width = middleGroundSpriteA.getGlobalBounds().width;
    middleGroundSpriteA.move(-movement,0);
    middleGroundSpriteB.move(-movement,0);
    if (middleGroundSpriteA.getPosition().x <= -width) {
        // Snap it to the right of Sprite B
        middleGroundSpriteA.setPosition(middleGroundSpriteB.getPosition().x + width, 0);
    }

    // 3. Check if Sprite B is off-screen
    if (middleGroundSpriteB.getPosition().x <= -width) {
        // Snap it to the right of Sprite A
        middleGroundSpriteB.setPosition(middleGroundSpriteA.getPosition().x + width, 0);
    }
}
void Game::updatePlaying(float dt) {

}
void Game::updatePaused(float dt) {

}
void Game::updateLevelMenu(float dt) {

}
void Game::updateGameOver(float dt) {

}
