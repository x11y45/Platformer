//
// Created by x11y45 on 4/4/26.
//

#ifndef PLATFORMER_LEVEL_H
#define PLATFORMER_LEVEL_H

#include <string>
#include <memory>
#include <SFML/Graphics.hpp>
#include "map.h"

enum class LevelState {
	NOT_STARTED,
	PLAYING,
	PAUSED,
	COMPLETED,
	FAILED
};

struct LevelConfig {
	std::string name;
	std::string csvPath;
	float timeLimit;              // Seconds
	int targetScore;            // the minimum score you need to pass the level
	sf::Vector2f playerSpawnPoint;
	sf::Vector2f goalPosition;
};

class Level {
public:
	virtual ~Level();
	
	// Lifecycle methods (pure virtual - must implement)
	virtual void load();
	
	// Lifecycle methods (default implementations provided)
	virtual void unload();
	
	// Game loop methods (default implementations)
	virtual void update(float dt);
	virtual void render(sf::RenderTarget& target, const sf::View& view);
	virtual void handleInput(const sf::Event& event);
	
	// Level logic (derived can override)
	virtual bool isCompleted() const;
	virtual bool isFailed() const;
	virtual void onPlayerDeath();
	virtual void onPlayerReachGoal();
	
	// State management
	void start();
	void pause();
	void resume();
	void reset();
	
	// Getters
	LevelState getState() const { return state; }
	int getScore() const { return score; }
	float getTimeRemaining() const;
	float getTimeElapsed() const { return timeElapsed; }
	const map& getMap() const { return levelMap; }
	map& getMap() { return levelMap; }
	const LevelConfig& getConfig() const { return config; }
	sf::Vector2f getPlayerSpawnPoint() const { return config.playerSpawnPoint; }
	sf::Vector2f getGoalPosition() const { return config.goalPosition; }
	
protected:
	Level(const std::string& levelName);

	void setState(LevelState newState);
	void addScore(int points);
	void resetTimer();
	void setConfig(const LevelConfig& cfg);
	
	// Level data
	LevelConfig config;
	map levelMap;
	// Future: EntityManager entityManager;
	
	LevelState state;
	int score;
	float timeElapsed;
	int playerDeaths;
	bool debugMode;
};

#endif //PLATFORMER_LEVEL_H
