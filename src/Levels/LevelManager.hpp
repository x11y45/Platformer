//
// Created by x11y45 on 4/4/26.
//

#ifndef PLATFORMER_LEVELMANAGER_H
#define PLATFORMER_LEVELMANAGER_H

#include <memory>
#include <map>
#include <string>
#include <vector>
#include "Level.hpp"


class LevelManager {
public:
	LevelManager();
	~LevelManager() = default;
	
	// Level registration/discovery
	void registerLevel(int levelId, const std::string& levelConfigPath);
	int autoDiscoverLevels(const std::string& levelsDirectory);
	std::vector<int> getRegisteredLevelIds() const;
	
	// Level management
	bool loadLevel(int levelId);
	void unloadCurrentLevel();
	void restartCurrentLevel();
	
	// Level lifecycle control
	void startCurrentLevel();
	void pauseCurrentLevel();
	void resumeCurrentLevel();
	
	// Update and render current level
	void update(float dt);
	void render(sf::RenderTarget& target, const sf::View& view);
	void handleInput(const sf::Event& event);
	
	// Getters
	Level* getCurrentLevel() const { return currentLevel.get(); }
	const Level* getCurrentLevelConst() const { return currentLevel.get(); }
	int getCurrentLevelId() const { return currentLevelId; }
	bool hasLevel() const { return currentLevel != nullptr; }
	
	// Level progression
	bool loadNextLevel();
	bool loadPreviousLevel();
	
private:
	std::unique_ptr<Level> currentLevel;
	int currentLevelId;
	std::map<int, std::string> levelConfigs;
};

#endif //PLATFORMER_LEVELMANAGER_H
