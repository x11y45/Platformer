#include "Game.hpp"

Game::Game()
    : window(sf::VideoMode(800, 600), "GothicVania Platformer")
      , worldView(sf::FloatRect(0.f, 0.f, 800.f, 600.f))
      , uiView(sf::FloatRect(0.f, 0.f, 800.f, 600.f))
      , camera(800.f, 600.f)
      , currentState(GameState::MAIN_MENU)
      , mainMenu(window.getSize())
      , pauseMenu(window.getSize())
      , levelMenu(window.getSize())
      , playerLives(3)
      , maxLives(3) {
    window.setFramerateLimit(60);
    levelManager.autoDiscoverLevels("assets/levels");
    levelMenu.init(levelManager);
    if (!levelManager.loadLevel(1)) {
        levelManager.loadNextLevel();
    }
    configureCameraForCurrentLevel();
    if (Level* level = getCurrentLevel()) {
        mainMenu.init(level->getMap());
    }
}

Level* Game::getCurrentLevel() {
    return levelManager.getCurrentLevel();
}

const Level* Game::getCurrentLevel() const {
    return levelManager.getCurrentLevelConst();
}

void Game::startLevel(int levelId) {
    if (levelManager.loadLevel(levelId)) {
        levelManager.startCurrentLevel();
        configureCameraForCurrentLevel();
        currentState = GameState::PLAYING;
    }
}

void Game::configureCameraForCurrentLevel() {
    Level* level = getCurrentLevel();
    if (!level) {
        return;
    }
    const sf::Vector2i grid = level->getMap().getGridDimensions();
    const float tileSize = static_cast<float>(level->getMap().getTileSize());
    camera.setBounds({0.f, 0.f, grid.x * tileSize, grid.y * tileSize});
    camera.setTarget(&level->getPlayerPositionRef());
    worldView = camera.getView();
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

        // Handle state-specific input
        switch (currentState) {
            case GameState::MAIN_MENU:
                if (event.type == sf::Event::MouseButtonPressed) {
                    switch (mainMenu.handleInput(worldMousePos)) {
                        case MainMenuAction::StartGame:
                            startLevel(levelManager.getCurrentLevelId() < 0 ? 1 : levelManager.getCurrentLevelId());
                            break;
                        case MainMenuAction::OpenLevelMenu:
                            currentState = GameState::LEVEL_MENU;
                            break;
                        case MainMenuAction::None:
                            break;
                    }
                }
                break;
            case GameState::PLAYING:
                // Pass all events to level (which passes to player)
                levelManager.handleInput(event);
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                    changeState(GameState::PAUSED);
                }
                break;
            case GameState::PAUSED:
                if (event.type == sf::Event::MouseButtonPressed) {
                    switch (pauseMenu.handleInput(worldMousePos)) {
                        case PauseMenuAction::Resume:
                            changeState(GameState::PLAYING);
                            break;
                        case PauseMenuAction::Restart:
                            levelManager.restartCurrentLevel();
                            configureCameraForCurrentLevel();
                            changeState(GameState::PLAYING);
                            break;
                        case PauseMenuAction::MainMenu:
                            currentState = GameState::MAIN_MENU;
                            break;
                        case PauseMenuAction::None:
                            break;
                    }
                }
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                    changeState(GameState::PLAYING);
                }
                break;
            case GameState::LEVEL_MENU:
                if (event.type == sf::Event::MouseButtonPressed) {
                    const LevelMenuAction action = levelMenu.handleInput(worldMousePos);
                    switch (action.type) {
                        case LevelMenuActionType::Back:
                            currentState = GameState::MAIN_MENU;
                            break;
                        case LevelMenuActionType::SelectLevel:
                            startLevel(action.levelId);
                            break;
                        case LevelMenuActionType::None:
                            break;
                    }
                }
                break;
            case GameState::GAME_OVER:
                if (event.type == sf::Event::MouseButtonPressed) {
                    handleGameOverInput();
                }
                break;
            default:
                break;
        }
    }
}
void Game::update(float dt) {
    switch (currentState) {
        case GameState::MAIN_MENU:
            // TODO: Update menu animations
            mainMenu.update(dt);
            break;

        case GameState::PLAYING:
            // Update level and player
            levelManager.update(dt);
            camera.update(dt);
            worldView = camera.getView();
            break;
            
        case GameState::PAUSED:
            // No updates while paused
            pauseMenu.update(dt);
            break;
            
        case GameState::LEVEL_MENU:
            levelMenu.update(dt);
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
            mainMenu.render(window);
            break;
            
        case GameState::PLAYING:
            // Render world
            window.setView(worldView);

            // Render map (parallax + tiles)
            levelManager.render(window, worldView);
            
            // Render UI
            window.setView(uiView);
            // TODO: Render HUD (health, lives)
            break;
            
        case GameState::PAUSED:
            // Render world (frozen)
            window.setView(worldView);
            window.setView(uiView);
            pauseMenu.render(window);
            break;
            
        case GameState::LEVEL_MENU:
            window.setView(uiView);
            levelMenu.render(window);
            break;
            
        case GameState::GAME_OVER:
            window.setView(uiView);
            // TODO: Render game over screen
            break;
    }
    window.display();
}

// input handling implementations
void Game::handlePlayingInput() {
    // Moved to processEvents - keyboard input handled there
}

void Game::handlePausedInput() {
    // Moved to processEvents - keyboard input handled there
}

void Game::handleGameOverInput() {
    // TODO: Handle retry/quit options
}

void Game::changeState(GameState newState) {
    currentState = newState;
    if (newState == GameState::PLAYING) {
        levelManager.startCurrentLevel();
        configureCameraForCurrentLevel();
    }
}
