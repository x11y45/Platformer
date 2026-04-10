//
// Created by x11y45 on 4/4/26.
//

#include "LevelManager.hpp"
#include <iostream>
#include <filesystem>
#include <regex>
#include <algorithm>

LevelManager::LevelManager()
	: currentLevel(nullptr)
	, currentLevelId(-1) {
}

void LevelManager::registerLevel(int levelId, const std::string& levelConfigPath) {
	levelConfigs[levelId] = levelConfigPath;
}

int LevelManager::autoDiscoverLevels(const std::string& levelsDirectory) {
	levelConfigs.clear();
	std::regex levelPattern(R"(level([0-9]+)\.txt$)");
	int discoveredCount = 0;

	try {
		for (const auto& entry : std::filesystem::directory_iterator(levelsDirectory)) {
			if (!entry.is_regular_file()) {
				continue;
			}

			const std::string filename = entry.path().filename().string();
			std::smatch match;
			if (!std::regex_search(filename, match, levelPattern)) {
				continue;
			}

			const int levelId = std::stoi(match[1].str());
			registerLevel(levelId, entry.path().string());
			discoveredCount++;
		}
	} catch (const std::exception& err) {
		std::cerr << "Failed to auto-discover levels in " << levelsDirectory
		          << ": " << err.what() << std::endl;
	}

	return discoveredCount;
}

std::vector<int> LevelManager::getRegisteredLevelIds() const {
	std::vector<int> levelIds;
	levelIds.reserve(levelConfigs.size());
	for (const auto& levelEntry : levelConfigs) {
		levelIds.push_back(levelEntry.first);
	}
	return levelIds;
}

bool LevelManager::loadLevel(int levelId) {
	const auto levelIt = levelConfigs.find(levelId);
	if (levelIt == levelConfigs.end()) {
		std::cerr << "Level ID " << levelId << " is not registered." << std::endl;
		return false;
	}

	const std::string& levelPath = levelIt->second;
	auto loadedLevel = std::make_unique<Level>(levelPath);
	currentLevel = std::move(loadedLevel);
	currentLevelId = levelId;
	return true;
}

void LevelManager::unloadCurrentLevel() {
	if (!currentLevel) {
		currentLevelId = -1;
		return;
	}
	currentLevel->unload();
	currentLevel.reset();
	currentLevelId = -1;
}

void LevelManager::restartCurrentLevel() {
	if (currentLevelId < 0) {
		return;
	}
	loadLevel(currentLevelId);
	startCurrentLevel();
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
	if (levelConfigs.empty()) {
		return false;
	}

	if (currentLevelId < 0) {
		return loadLevel(levelConfigs.begin()->first);
	}

	auto currentIt = levelConfigs.upper_bound(currentLevelId);
	if (currentIt == levelConfigs.end()) {
		return false;
	}

	return loadLevel(currentIt->first);
}

bool LevelManager::loadPreviousLevel() {
	if (levelConfigs.empty() || currentLevelId < 0) {
		return false;
	}

	auto currentIt = levelConfigs.lower_bound(currentLevelId);
	if (currentIt == levelConfigs.begin()) {
		return false;
	}
	--currentIt;
	return loadLevel(currentIt->first);
}
