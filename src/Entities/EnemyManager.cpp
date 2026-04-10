//
// Created by x11y45 on 4/9/26.
//

#include "EnemyManager.h"
#include "Levels/map.h"
#include <algorithm>
#include <iostream>
#include <utility>

void EnemyManager::load(
	const std::map<std::string, EnemyTemplate>& templateData,
	const std::map<int, std::string>& spawnMarkerData,
	map& levelMap
) {
	templates = templateData;
	spawnMarkers = spawnMarkerData;
	enemies.clear();
	spawnFromMap(levelMap);
}

void EnemyManager::clear() {
	enemies.clear();
}

void EnemyManager::spawnFromMap(map& levelMap) {
	const sf::Vector2i grid = levelMap.getGridDimensions();
	for (int row = 0; row < grid.y; ++row) {
		for (int col = 0; col < grid.x; ++col) {
			Tile* tile = levelMap.getTileAtGrid(col, row);
			if (!tile) {
				continue;
			}

			const auto spawnMarkerIt = spawnMarkers.find(tile->id);
			if (spawnMarkerIt == spawnMarkers.end()) {
				continue;
			}

			const auto templateIt = templates.find(spawnMarkerIt->second);
			if (templateIt == templates.end()) {
				std::cerr << "Enemy template not found for spawn marker " << tile->id << std::endl;
				continue;
			}

			const sf::FloatRect tileBounds = levelMap.getTileWorldBounds(*tile);
			//Enemy *enemy = new Enemy(templateIt->second, sf::Vector2f(tileBounds.left, tileBounds.top));
			auto enemy = std::make_unique<Enemy>(templateIt->second, sf::Vector2f(tileBounds.left, tileBounds.top));
			enemy->setLevelMap(&levelMap);
			enemies.push_back(std::move(enemy));
		}
	}
}

void EnemyManager::update(float dt, map& levelMap, const sf::Vector2f& playerPosition) {
	for (auto& enemy : enemies) {
		enemy->setLevelMap(&levelMap);
		enemy->setTargetPlayerPosition(&playerPosition);
		enemy->update(dt);
	}

	enemies.erase(
		std::remove_if(enemies.begin(), enemies.end(), [](const std::unique_ptr<Enemy>& enemy) {
			return !enemy->isAlive();
		}),
		enemies.end()
	);
}

void EnemyManager::render(sf::RenderTarget& target) {
	for (auto& enemy : enemies) {
		enemy->render(target);
	}
}

std::size_t EnemyManager::getEnemyCount() const {
	return enemies.size();
}
