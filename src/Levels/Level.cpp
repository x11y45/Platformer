//
// Created by x11y45 on 4/4/26.
//

#include "Level.hpp"
#include <sstream>
#include <stdexcept>

namespace {

TileType parseTileType(const std::string& value) {
	if (value == "EMPTY") {
		return TileType::EMPTY;
	}
	if (value == "PLATFORM") {
		return TileType::PLATFORM;
	}
	if (value == "SPIKE") {
		return TileType::SPIKE;
	}
	if (value == "WALL") {
		return TileType::WALL;
	}
	if (value == "COLLECTIBLE") {
		return TileType::COLLECTIBLE;
	}

	throw std::runtime_error("Unknown tile type: " + value);
}

bool parseBool(const std::string& value) {
	if (value == "1" || value == "true" || value == "TRUE") {
		return true;
	}
	if (value == "0" || value == "false" || value == "FALSE") {
		return false;
	}

	throw std::runtime_error("Invalid boolean value: " + value);
}

std::vector<std::string> splitCommaSeparated(const std::string& value) {
	std::vector<std::string> parts;
	std::stringstream ss(value);
	std::string part;
	while (std::getline(ss, part, ',')) {
		parts.push_back(part);
	}
	return parts;
}

AnimationSpec parseAnimationSpec(const std::string& value, const std::string& context) {
	const std::vector<std::string> parts = splitCommaSeparated(value);
	if (parts.size() != 2 && parts.size() != 3) {
		throw std::runtime_error("Invalid animation definition in " + context + ": " + value);
	}

	AnimationSpec spec;
	spec.path = parts[0];
	spec.frameCount = std::stoi(parts[1]);
	if (parts.size() == 3) {
		spec.frameDuration = std::stof(parts[2]);
	}
	return spec;
}

}

Level::Level(std::string configPath):
	levelConfigPath(std::move(configPath)),
	state(LevelState::NOT_STARTED),
	score(0),
	timeElapsed(0.0f),
	playerDeaths(0),
	playerDeathNotified(false),
	debugMode(false) {
	load();
}
void Level::load() {
	config = LevelConfig{};
	levelMap.clear();
	enemyManager.clear();
	playerDeathNotified = false;
	// Load Level configuration from file txt
	std::ifstream configFile(levelConfigPath);
	if (!configFile.is_open()) {
		std::cerr << "Failed to open level config file: " << levelConfigPath << std::endl;
		return;
	}

	std::string line;
	while (std::getline(configFile, line)) {
		if (line.empty() || line[0] == '#') {
			continue; // Skip empty lines and comments
		}

		size_t delimiterPos = line.find('=');
		if (delimiterPos != std::string::npos) {
			std::string key = line.substr(0, delimiterPos);
			std::string value = line.substr(delimiterPos + 1);

			if (key == "name") {
				config.name = value;
			} else if (key == "mapCSV") {
				config.mapCSVPath = value;
			} else if (key.rfind("Tile.", 0) == 0) {
				const std::string idText = key.substr(5);
				const std::vector<std::string> parts = splitCommaSeparated(value);
				if (parts.size() != 4 && parts.size() != 5) {
					std::cerr << "Invalid tile definition in level config: " << line << std::endl;
				} else {
					try {
						const int tileId = std::stoi(idText);
						TileDefinition definition;
						definition.id = tileId;
						definition.imagePath = parts[0];
						definition.type = parseTileType(parts[1]);
						definition.isSolid = parseBool(parts[2]);
						definition.isHarmful = parseBool(parts[3]);
						definition.isSpawnMarker = parts.size() == 5 ? parseBool(parts[4]) : false;
						config.tileDefinitions[tileId] = definition;
					} catch (const std::exception& err) {
						std::cerr << "Failed to parse tile definition: " << err.what() << std::endl;
					}
				}
			} else if (key.rfind("EnemyTemplate.", 0) == 0) {
				const std::string remainder = key.substr(14);
				const size_t statsPos = remainder.find(".Stats");
				const size_t animPos = remainder.find(".Anim.");
				const size_t attackHitboxPos = remainder.find(".AttackHitbox");

				if (statsPos != std::string::npos && statsPos + 6 == remainder.size()) {
					const std::string templateName = remainder.substr(0, statsPos);
					const std::vector<std::string> parts = splitCommaSeparated(value);
					if (parts.size() != 11 && parts.size() != 12 && parts.size() != 13) {
						std::cerr << "Invalid enemy stats definition in level config: " << line << std::endl;
					} else {
						try {
							EnemyTemplate& enemyTemplate = config.enemyTemplates[templateName];
							enemyTemplate.name = templateName;
							enemyTemplate.damage = std::stoi(parts[0]);
							enemyTemplate.speed = std::stof(parts[1]);
							enemyTemplate.health = std::stoi(parts[2]);
							enemyTemplate.canMove = parseBool(parts[3]);
							enemyTemplate.canJump = parseBool(parts[4]);
							enemyTemplate.canAttack = parseBool(parts[5]);
							enemyTemplate.jumpStrength = std::stof(parts[6]);
							enemyTemplate.hitboxSize = {std::stof(parts[7]), std::stof(parts[8])};
							enemyTemplate.hitboxOffset = {std::stof(parts[9]), std::stof(parts[10])};
							enemyTemplate.renderScale = parts.size() >= 12 ? std::stof(parts[11]) : 1.f;
							enemyTemplate.flipWhenFacingRight = parts.size() == 13 ? parseBool(parts[12]) : false;
						} catch (const std::exception& err) {
							std::cerr << "Failed to parse enemy stats: " << err.what() << std::endl;
						}
					}
				} else if (animPos != std::string::npos) {
					const std::string templateName = remainder.substr(0, animPos);
					const std::string animationName = remainder.substr(animPos + 6);
					try {
						EnemyTemplate& enemyTemplate = config.enemyTemplates[templateName];
						enemyTemplate.name = templateName;
						enemyTemplate.animations[animationName] = parseAnimationSpec(value, line);
					} catch (const std::exception& err) {
						std::cerr << "Failed to parse enemy animation: " << err.what() << std::endl;
					}
				} else if (attackHitboxPos != std::string::npos && attackHitboxPos + 13 == remainder.size()) {
					const std::string templateName = remainder.substr(0, attackHitboxPos);
					const std::vector<std::string> parts = splitCommaSeparated(value);
					if (parts.size() != 4) {
						std::cerr << "Invalid enemy attack hitbox definition in level config: " << line << std::endl;
					} else {
						try {
							EnemyTemplate& enemyTemplate = config.enemyTemplates[templateName];
							enemyTemplate.name = templateName;
							enemyTemplate.attackHitboxOffset = {std::stof(parts[0]), std::stof(parts[1])};
							enemyTemplate.attackHitboxSize = {std::stof(parts[2]), std::stof(parts[3])};
							enemyTemplate.hasAttackHitbox = true;
						} catch (const std::exception& err) {
							std::cerr << "Failed to parse enemy attack hitbox: " << err.what() << std::endl;
						}
					}
				}
			} else if (key.rfind("EnemySpawn.", 0) == 0) {
				const std::string markerText = key.substr(11);
				try {
					const int markerId = std::stoi(markerText);
					config.enemySpawnMarkers[markerId] = value;
				} catch (const std::exception& err) {
					std::cerr << "Failed to parse enemy spawn marker: " << err.what() << std::endl;
				}
			} else if (key == "timeLimit") {
				config.timeLimit = std::stof(value);
			} else if (key == "targetScore") {
				config.targetScore = std::stoi(value);
			} else if (key.rfind("Player.", 0) == 0) {
				std::string animName = key.substr(7); // Remove "Player."
				try {
					config.Player[animName] = parseAnimationSpec(value, line);
				} catch (const std::exception& err) {
					std::cerr << "Failed to parse player animation: " << err.what() << std::endl;
				}
			} else if (key.rfind("Enemy.", 0) == 0) {
				std::string enemyName = key.substr(6); // Remove "Enemy."
				try {
					config.Enemies[enemyName] = parseAnimationSpec(value, line);
				} catch (const std::exception& err) {
					std::cerr << "Failed to parse enemy animation: " << err.what() << std::endl;
				}
			} else if (key.rfind("MapLayer.", 0) == 0) {
				int layerNum = std::stoi(key.substr(9)); // Remove "MapLayer."
				size_t layerDelimiter = value.find(',');
				if (layerDelimiter != std::string::npos) {
					std::string layerPath = value.substr(0, layerDelimiter);
					float parallaxFactor = std::stof(value.substr(layerDelimiter + 1));
					config.mapLayers[layerNum] = {layerPath, parallaxFactor};
					std::cout << "Adding map layer: " << layerNum << " with path: " << layerPath
					          << " and parallax factor: " << parallaxFactor << std::endl;
				}
			}
		}
	}

	if (config.mapCSVPath.empty()) {
		std::cerr << "No map CSV defined in level config: " << levelConfigPath << std::endl;
	} else if (config.tileDefinitions.empty()) {
		std::cerr << "No tile definitions defined in level config: " << levelConfigPath << std::endl;
	} else {
		levelMap.init(config.tileDefinitions, config.mapLayers);
		if (!levelMap.loadFromCSV(config.mapCSVPath)) {
			std::cerr << "Failed to load level map CSV: " << config.mapCSVPath << std::endl;
		} else {
			if (config.Player.empty()) {
				std::cerr << "No player animations defined in level config!" << std::endl;
			} else {
				player.init(config.Player, config.playerName);
			}
			enemyManager.load(config.enemyTemplates, config.enemySpawnMarkers, levelMap);
		}
	}
}
void Level::unload() {
	levelMap.clear();
	enemyManager.clear();
	setState(LevelState::NOT_STARTED);
	score = 0;
	timeElapsed = 0.0f;
	playerDeaths = 0;
	playerDeathNotified = false;
}

void Level::update(float dt) {
	if (state != LevelState::PLAYING) {
		return;
	}

	player.update(dt);
	CollisionResult collisionResult = collisionSystem.resolveMapCollision(player.getBounds(), levelMap);
	player.updateMovementsStates(collisionResult);

	const sf::Vector2i grid = levelMap.getGridDimensions();
	if (grid.y > 0 && levelMap.getTileSize() > 0) {
		const float mapBottom = static_cast<float>(grid.y * levelMap.getTileSize());
		if (player.getBounds().top > mapBottom) {
			player.takeDamage(static_cast<int>(player.getHealth()), HitboxDirection::None);
		}
	}

	enemyManager.resolvePlayerAttack(player.getAttackInfo());
	player.finalizeAttackFrame();
	enemyManager.update(dt, levelMap, player);
	if (enemyManager.consumeBossDefeatedEvent()) {
		onPlayerReachGoal();
	}

	if (!playerDeathNotified && player.isDeathAnimationFinished()) {
		onPlayerDeath();
		playerDeathNotified = true;
	}

	timeElapsed += dt;
}


void Level::render(sf::RenderTarget& target, const sf::View& view) {
	// Render map (parallax + tiles)
	levelMap.render(target, view);

	// Render player
	enemyManager.render(target);
	player.render(target);
}

void Level::handleInput(const sf::Event& event) {
	// Pass input to player
	player.handleInput(event);
	
	// Level-specific input
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
	return state == LevelState::FAILED;
}

void Level::onPlayerDeath() {
	playerDeaths++;
	setState(LevelState::FAILED);
	std::cout << "Player died. Deaths: " << playerDeaths << std::endl;
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
		playerDeathNotified = false;
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
