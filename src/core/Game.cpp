#include "Game.hpp"
#include <iostream>

Game::Game()
    : window(sf::VideoMode(800, 600), "GothicVania Platformer")
      , worldView(sf::FloatRect(0.f, 0.f, 800.f, 600.f))
      , uiView(sf::FloatRect(0.f, 0.f, 800.f, 600.f))
      , camera(800.f, 600.f)
      , currentState(GameState::MAIN_MENU)
      , mainMenu(window.getSize())
      , pauseMenu(window.getSize())
      , levelMenu(window.getSize())
      , gameOverScreen(window.getSize())
      , playerLives(3)
      , maxLives(3) {
    window.setFramerateLimit(60);
    levelManager.autoDiscoverLevels("assets/levels");
    levelMenu.init(levelManager);
    if (!levelManager.loadLevel(1)) {
        levelManager.loadNextLevel();
    }
    loadAudioAssets();
    configureCameraForCurrentLevel();
    if (Level* level = getCurrentLevel()) {
        mainMenu.init(level->getMap());
        lastHealingEventCount = level->player.getHealingStartedEvents();
        lastHurtEventCount = level->player.getHurtEvents();
    }
    updateAudioForState();
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
        if (Level* level = getCurrentLevel()) {
            lastHealingEventCount = level->player.getHealingStartedEvents();
            lastHurtEventCount = level->player.getHurtEvents();
        }
        updateAudioForState();
    }
}

void Game::configureCameraForCurrentLevel() {
    Level* level = getCurrentLevel();
    if (!level) {
        return;
    }
    const sf::Vector2i grid = level->getMap().getGridDimensions();
    const float tileSize = static_cast<float>(level->getMap().getTileSize());
    camera.setBounds({tileSize, 0.f, (grid.x-2) * tileSize, grid.y * tileSize});
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


        switch (currentState) {
            case GameState::MAIN_MENU:
                if (event.type == sf::Event::MouseButtonPressed) {
                    switch (mainMenu.handleInput(worldMousePos)) {
                        case MainMenuAction::StartGame:
                            startLevel(levelManager.getCurrentLevelId() < 0 ? 1 : levelManager.getCurrentLevelId());
                            break;
                        case MainMenuAction::OpenLevelMenu:
                            changeState(GameState::LEVEL_MENU);
                            break;
                        case MainMenuAction::None:
                            break;
                    }
                }
                break;
            case GameState::PLAYING:
                if (event.type == sf::Event::MouseButtonPressed &&
                    hud.handleInput(worldMousePos) == HUDAction::Pause) {
                    currentState = GameState::PAUSED;
                    break;
                }
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
                            changeState(GameState::MAIN_MENU);
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
                            changeState(GameState::MAIN_MENU);
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
                    switch (gameOverScreen.handleInput(worldMousePos)) {
                        case GameOverScreenAction::Retry:
                            levelManager.restartCurrentLevel();
                            configureCameraForCurrentLevel();
                            changeState(GameState::PLAYING);
                            break;
                        case GameOverScreenAction::MainMenu:
                            changeState(GameState::MAIN_MENU);
                            break;
                        case GameOverScreenAction::None:
                            break;
                    }
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
            mainMenu.update(dt, window.mapPixelToCoords(sf::Mouse::getPosition(window), uiView));
            break;

        case GameState::PLAYING:
            levelManager.update(dt);
            if (const Level* level = getCurrentLevel(); level) {
                if (level->getState() == LevelState::FAILED) {
                    changeState(GameState::GAME_OVER);
                } else if (level->getState() == LevelState::COMPLETED) {
                    changeState(GameState::LEVEL_MENU);
                }
            }
            if (Level* level = getCurrentLevel()) {
                const unsigned int healingEvents = level->player.getHealingStartedEvents();
                const unsigned int hurtEvents = level->player.getHurtEvents();
                if (healingEvents != lastHealingEventCount) {
                    healingSound.stop();
                    healingSound.play();
                    lastHealingEventCount = healingEvents;
                }
                if (hurtEvents != lastHurtEventCount) {
                    hurtSound.stop();
                    hurtSound.play();
                    lastHurtEventCount = hurtEvents;
                }
            }
            hud.update(dt, window.mapPixelToCoords(sf::Mouse::getPosition(window), uiView));
            camera.update(dt);
            worldView = camera.getView();
            break;

        case GameState::PAUSED:
            pauseMenu.update(dt, window.mapPixelToCoords(sf::Mouse::getPosition(window), uiView));
            break;

        case GameState::LEVEL_MENU:
            levelMenu.update(dt, window.mapPixelToCoords(sf::Mouse::getPosition(window), uiView));
            break;

        case GameState::GAME_OVER:
            gameOverScreen.update(dt, window.mapPixelToCoords(sf::Mouse::getPosition(window), uiView));
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

            window.setView(worldView);


            levelManager.render(window, worldView);


            window.setView(uiView);
            if (const Level* level = getCurrentLevel()) {
                hud.render(window,  level->player);
            }
            break;

        case GameState::PAUSED:

            window.setView(worldView);
            levelManager.render(window, worldView);
            window.setView(uiView);
            if (const Level* level = getCurrentLevel()) {
                hud.render(window,  level->player);
            }

            window.setView(uiView);
            pauseMenu.render(window);
            break;

        case GameState::LEVEL_MENU:
            window.setView(uiView);
            levelMenu.render(window);
            break;

        case GameState::GAME_OVER:
            window.setView(uiView);
            gameOverScreen.render(window);
            break;
    }
    window.display();
}

void Game::changeState(GameState newState) {
    currentState = newState;
    if (newState == GameState::PLAYING) {
        levelManager.startCurrentLevel();
        configureCameraForCurrentLevel();
        if (Level* level = getCurrentLevel()) {
            lastHealingEventCount = level->player.getHealingStartedEvents();
            lastHurtEventCount = level->player.getHurtEvents();
        }
    }
    updateAudioForState();
}

bool Game::loadAudioAssets() {
    bool loaded = true;
    if (!menuMusic.openFromFile("assets/audio/menu.wav")) {
        std::cerr << "Failed to load menu music: assets/audio/menu.wav\n";
        loaded = false;
    }
    if (!levelMusic.openFromFile("assets/audio/playing.wav")) {
        std::cerr << "Failed to load level music: assets/audio/playing.wav\n";
        loaded = false;
    }
    if (!healingBuffer.loadFromFile("assets/audio/healing.wav")) {
        std::cerr << "Failed to load healing sfx: assets/audio/healing.wav\n";
        loaded = false;
    }
    if (!hurtBuffer.loadFromFile("assets/audio/hurt.wav")) {
        std::cerr << "Failed to load hurt sfx: assets/audio/hurt.wav\n";
        loaded = false;
    }

    healingSound.setBuffer(healingBuffer);
    hurtSound.setBuffer(hurtBuffer);
    menuMusic.setLoop(true);
    levelMusic.setLoop(true);
    return loaded;
}

void Game::updateAudioForState() {
    const bool playLevelMusic = currentState == GameState::PLAYING && levelManager.getCurrentLevelId() == 1;
    const bool playMenuMusic = !playLevelMusic;

    if (playMenuMusic) {
        if (levelMusic.getStatus() == sf::Music::Playing) {
            levelMusic.stop();
        }
        if (menuMusic.getStatus() != sf::Music::Playing) {
            menuMusic.play();
        }
        return;
    }

    if (menuMusic.getStatus() == sf::Music::Playing) {
        menuMusic.stop();
    }
    if (levelMusic.getStatus() != sf::Music::Playing) {
        levelMusic.play();
    }
}
