//
// Created by x11y45 on 4/9/26.
//

#ifndef PLATFORMER_ENEMYMANAGER_H
#define PLATFORMER_ENEMYMANAGER_H

#include <memory>
#include <map>
#include <vector>
#include "Enemy.h"

class map;

class EnemyManager {
public:
	void load(
		const std::map<std::string, EnemyTemplate>& templates,
		const std::map<int, std::string>& spawnMarkers,
		map& levelMap
	);
	void clear();
	void update(float dt, map& levelMap, const sf::Vector2f& playerPosition);
	void render(sf::RenderTarget& target);
	std::size_t getEnemyCount() const;

private:
	void spawnFromMap(map& levelMap);

	std::map<std::string, EnemyTemplate> templates;
	std::map<int, std::string> spawnMarkers;
	std::vector<std::unique_ptr<Enemy>> enemies;
};

#endif //PLATFORMER_ENEMYMANAGER_H
