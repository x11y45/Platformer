



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


	void registerLevel(int levelId, const std::string& levelConfigPath);
	int autoDiscoverLevels(const std::string& levelsDirectory);
	std::vector<int> getRegisteredLevelIds() const;


	bool loadLevel(int levelId);
	void unloadCurrentLevel();
	void restartCurrentLevel();


	void startCurrentLevel();
	void pauseCurrentLevel();
	void resumeCurrentLevel();


	void update(float dt);
	void render(sf::RenderTarget& target, const sf::View& view);
	void handleInput(const sf::Event& event);


	Level* getCurrentLevel() const { return currentLevel.get(); }
	const Level* getCurrentLevelConst() const { return currentLevel.get(); }
	int getCurrentLevelId() const { return currentLevelId; }
	bool hasLevel() const { return currentLevel != nullptr; }


	bool loadNextLevel();
	bool loadPreviousLevel();

private:
	std::unique_ptr<Level> currentLevel;
	int currentLevelId;
	std::map<int, std::string> levelConfigs;
};

#endif //PLATFORMER_LEVELMANAGER_H
