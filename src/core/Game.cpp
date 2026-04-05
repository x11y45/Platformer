#include "Game.hpp"

Game::Game()
    : window(sf::VideoMode(800, 600), "GothicVania Platformer")
      , worldView(sf::FloatRect(0.f, 0.f, 800.f, 600.f))
      , uiView(sf::FloatRect(0.f, 0.f, 800.f, 600.f))
      , camera(800.f, 600.f)
      , currentState(GameState::MAIN_MENU)
      , mainMenu(window.getSize()), playerLives(3)
      , maxLives(3) {
    window.setFramerateLimit(60);
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
            mainMenu.update(dt);
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
                mainMenu.render(window);
            break;
            
        case GameState::PLAYING:
            // Render world
            window.setView(worldView);
            // Render map (parallax + tiles)
            gameMap.render(window, worldView);
            // TODO: Render entities
            
            // Render UI
            window.setView(uiView);
            // TODO: Render HUD (health, lives)
            break;
            
        case GameState::PAUSED:
            // Render world (frozen)
            window.setView(worldView);
            // Render map (parallax + tiles)
            gameMap.render(window, worldView);
            // TODO: Render entities
            
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
    if (mainMenu.MenuButtonSprite.getGlobalBounds().contains(worldMousePos)) {
        changeState(GameState::MAIN_MENU);
    } else if (mainMenu.PlayButtonSprite.getGlobalBounds().contains(worldMousePos)) {
        changeState(GameState::PLAYING);
    } else if (mainMenu.LevelButtonSprite.getGlobalBounds().contains(worldMousePos)) {
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
