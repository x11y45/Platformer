//
// Created by x11y45 on 4/4/26.
//

#ifndef PLATFORMER_LEVEL_H
#define PLATFORMER_LEVEL_H

#include <string>
#include <memory>
#include <SFML/Graphics.hpp>
#include "map.h"
#include "Entities/Player.h"
#include "Entities/EnemyManager.h"
#include "Entities/EnemyConfig.h"
#include "Entities/AnimationSpec.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include "Physics/Collision.h"

enum class LevelState {
	NOT_STARTED,
	PLAYING,
	PAUSED,
	COMPLETED,
	FAILED
};

struct LevelConfig {
	std::string name;
	std::string mapCSVPath;
	std::map<int, TileDefinition> tileDefinitions;
	std::map<std::string, EnemyTemplate> enemyTemplates;
	std::map<int, std::string> enemySpawnMarkers;
	std::map<std::string, AnimationSpec> Player;
	std::string playerName;
	std::map<std::string, AnimationSpec> Enemies;
	std::map<int,std::pair<std::string, float>> mapLayers; // Layer path + parallax factor
	float timeLimit;              // Seconds
	int targetScore;            // the minimum score you need to pass the level
};

class Level {
public:
	~Level() = default;
	explicit Level(std::string configPath = "assets/levels/level1.txt");

	void load();

	void unload();
	
	// Game loop methods (default implementations)
	void update(float dt);
	void render(sf::RenderTarget& target, const sf::View& view);
	void handleInput(const sf::Event& event);

	// Level logic (derived can override)
	bool isCompleted() const;
	bool isFailed() const;
	void onPlayerDeath();
	void onPlayerReachGoal();
	
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
	const sf::Vector2f& getPlayerPositionRef() const { return player.getPositionRef(); }
	unsigned int getPlayerHealth() const { return player.getHealth(); }
	unsigned int getPlayerMaxHealth() const { return player.getMaxHealth(); }
	const LevelConfig& getConfig() const { return config; }
	const std::string& getConfigPath() const { return levelConfigPath; }
	sf::Vector2f getPlayerSpawnPoint();
	sf::Vector2f getGoalPosition();
	
public:

	void setState(LevelState newState);
	void addScore(int points);
	void resetTimer();
	void setConfig(const LevelConfig& cfg);
	void setConfigPath(const std::string& configPath) { levelConfigPath = configPath; }
	
	// Level data
	std::string levelConfigPath;
	LevelConfig config;
	map levelMap;
	// Future: EntityManager entityManager;
	Player player;
	EnemyManager enemyManager;
	LevelState state;
	Collision collisionSystem;
	int score;
	float timeElapsed;
	int playerDeaths;
	bool playerDeathNotified;
	bool debugMode;
};

#endif //PLATFORMER_LEVEL_H
