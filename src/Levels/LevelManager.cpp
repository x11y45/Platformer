//
// Created by x11y45 on 4/4/26.
//

#include "LevelManager.hpp"
#include <iostream>

LevelManager::LevelManager()
	: currentLevel(nullptr)
	, currentLevelId(-1) {
}

LevelManager::~LevelManager() {
	unloadCurrentLevel();
}

void LevelManager::registerLevel(int levelId, Level &level) {
	levelFactories[levelId] = &level;
	std::cout << "Registered level ID: " << levelId << std::endl;
}

bool LevelManager::loadLevel(int levelId) {
	// Check if level factory exists
	auto it = levelFactories.find(levelId);
	if (it == levelFactories.end()) {
		std::cerr << "Level ID " << levelId << " not registered!" << std::endl;
		return false;
	}

	unloadCurrentLevel();

	try {
		currentLevel = it->second;
		if (currentLevel) {
			currentLevel->load();
			currentLevelId = levelId;
			std::cout << "Loaded level: " << currentLevel->getConfig().name 
			          << " (ID: " << levelId << ")" << std::endl;
			return true;
		}
	} catch (const std::exception& err) {
		std::cerr << "Failed to load level " << levelId << ": " << err.what() << std::endl;
		currentLevel->reset();
		currentLevelId = -1;
		return false;
	}
	return false;
}

void LevelManager::unloadCurrentLevel() {
	if (currentLevel) {
		std::cout << "Unloading level: " << currentLevel->getConfig().name << std::endl;
		currentLevel->unload();
		currentLevel->reset();
		currentLevelId = -1;
	}
}

void LevelManager::restartCurrentLevel() {
	if (currentLevel) {
		std::cout << "Restarting level: " << currentLevel->getConfig().name << std::endl;
		currentLevel->reset();
		currentLevel->start();
	}
}

void LevelManager::startCurrentLevel() {
	if (currentLevel) {
		currentLevel->start();
	}
}

void LevelManager::pauseCurrentLevel() {
	if (currentLevel) {
		currentLevel->pause();
	}
}

void LevelManager::resumeCurrentLevel() {
	if (currentLevel) {
		currentLevel->resume();
	}
}

void LevelManager::update(float dt) {
	if (currentLevel) {
		currentLevel->update(dt);
	}
}

void LevelManager::render(sf::RenderTarget& target, const sf::View& view) {
	if (currentLevel) {
		currentLevel->render(target, view);
	}
}

void LevelManager::handleInput(const sf::Event& event) {
	if (currentLevel) {
		currentLevel->handleInput(event);
	}
}

bool LevelManager::loadNextLevel() {
	int nextLevelId = currentLevelId + 1;
	
	// Check if next level exists
	if (levelFactories.find(nextLevelId) != levelFactories.end()) {
		return loadLevel(nextLevelId);
	}
	
	std::cout << "No next level available (current: " << currentLevelId << ")" << std::endl;
	return false;
}

bool LevelManager::loadPreviousLevel() {
	int prevLevelId = currentLevelId - 1;
	
	// Check if previous level exists
	if (prevLevelId >= 0 && levelFactories.find(prevLevelId) != levelFactories.end()) {
		return loadLevel(prevLevelId);
	}
	
	std::cout << "No previous level available (current: " << currentLevelId << ")" << std::endl;
	return false;
}
