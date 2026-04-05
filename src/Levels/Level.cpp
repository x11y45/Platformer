//
// Created by x11y45 on 4/4/26.
//

#include "Level.hpp"
#include <iostream>

Level::Level(const std::string& levelName)
	: state(LevelState::NOT_STARTED)
	, score(0)
	, timeElapsed(0.0f)
	, playerDeaths(0)
	, debugMode(false) {
	config.name = levelName;
	config.timeLimit = 0;
	config.targetScore = 0;
	config.playerSpawnPoint = sf::Vector2f(100.0f, 100.0f);
	config.goalPosition = sf::Vector2f(1000.0f, 500.0f);
}

void Level::unload() {
	levelMap.clear();
	setState(LevelState::NOT_STARTED);
	score = 0;
	timeElapsed = 0.0f;
	playerDeaths = 0;
}

void Level::update(float dt) {
	if (state != LevelState::PLAYING) {
		return;
	}
	timeElapsed += dt;

	if (config.timeLimit > 0 && timeElapsed >= config.timeLimit) {
		setState(LevelState::FAILED);
		std::cout << "Level failed: Time limit exceeded" << std::endl;
	}else if (isCompleted()) {
		setState(LevelState::COMPLETED);
		std::cout << "Level completed! Score: " << score << std::endl;
	}else if (isFailed()) {
		setState(LevelState::FAILED);
		std::cout << "Level failed!" << std::endl;
	}
	
	// Update map parallax (will be done by game passing camera position)
	// levelMap.updateParallax() is called from game update
	
	// Future: Update entities
	// entityManager.update(dt);
}

void Level::render(sf::RenderTarget& target, const sf::View& view) {
	// Render the map (parallax + tiles)
	levelMap.render(target, view);
	
	// Future: Render entities
	// entityManager.render(target);
	
	// Render goal marker (simple visualization)
	if (debugMode) {
		sf::CircleShape goalMarker(20.0f);
		goalMarker.setFillColor(sf::Color(255, 215, 0, 128)); // Semi-transparent gold
		goalMarker.setPosition(config.goalPosition.x - 20.0f, config.goalPosition.y - 20.0f);
		target.draw(goalMarker);
		
		// Render spawn point marker
		sf::CircleShape spawnMarker(15.0f);
		spawnMarker.setFillColor(sf::Color(0, 255, 0, 128)); // Semi-transparent green
		spawnMarker.setPosition(config.playerSpawnPoint.x - 15.0f, config.playerSpawnPoint.y - 15.0f);
		target.draw(spawnMarker);
	}
}

void Level::handleInput(const sf::Event& event) {
	// Base implementation does nothing
	// Derived levels can override for level-specific input handling
	
	// Example: Toggle debug mode
	if (event.type == sf::Event::KeyPressed) {
		if (event.key.code == sf::Keyboard::F3) {
			debugMode = !debugMode;
		}
	}
}

bool Level::isCompleted() const {
	// Default: Level is completed when target score is reached
	// Derived classes can override for custom win conditions
	if (config.targetScore > 0) {
		return score >= config.targetScore;
	}
	
	// If no target score, level completion must be triggered manually
	// (e.g., player reaches goal position, handled by game logic)
	return false;
}
bool Level::isFailed() const {
	// Default: No failure condition
	// Derived classes can override (e.g., too many deaths, specific events)
	return false;
}

void Level::onPlayerDeath() {
	playerDeaths++;
	std::cout << "Player died. Deaths: " << playerDeaths << std::endl;
	// TODO:
}
void Level::onPlayerReachGoal() {
	setState(LevelState::COMPLETED);
	std::cout << "Player reached goal! Level completed." << std::endl;
}

void Level::start() {
	if (state == LevelState::NOT_STARTED || state == LevelState::FAILED) {
		setState(LevelState::PLAYING);
		resetTimer();
		score = 0;
		playerDeaths = 0;
		std::cout << "Level started: " << config.name << std::endl;
	}
}
void Level::pause() {
	if (state == LevelState::PLAYING) {
		setState(LevelState::PAUSED);
	}
}
void Level::resume() {
	if (state == LevelState::PAUSED) {
		setState(LevelState::PLAYING);
	}
}
void Level::reset() {
	unload();
	load();
	setState(LevelState::NOT_STARTED);
}

float Level::getTimeRemaining() const {
	if (config.timeLimit <= 0) {
		return -1.0f; // No time limit
	}
	return std::max(0.0f, static_cast<float>(config.timeLimit) - timeElapsed);
}

void Level::setState(LevelState newState) {
	if (state != newState) {
		state = newState;
		
		if (debugMode) {
			std::cout << "Level state changed to: ";
			switch (newState) {
				case LevelState::NOT_STARTED: std::cout << "NOT_STARTED"; break;
				case LevelState::PLAYING: std::cout << "PLAYING"; break;
				case LevelState::PAUSED: std::cout << "PAUSED"; break;
				case LevelState::COMPLETED: std::cout << "COMPLETED"; break;
				case LevelState::FAILED: std::cout << "FAILED"; break;
			}
			std::cout << std::endl;
		}
	}
}
void Level::setConfig(const LevelConfig& cfg) {
	config = cfg;
}

void Level::addScore(int points) {
	score += points;
	if (score < 0) {
		score = 0; // Don't allow negative scores
	}
}

void Level::resetTimer() {
	timeElapsed = 0.0f;
}


