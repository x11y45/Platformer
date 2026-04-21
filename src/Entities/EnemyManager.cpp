//
// Created by x11y45 on 4/9/26.
//

#include "EnemyManager.h"
#include "Player.h"
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
	nextEnemyId = 0;
	activeAttackSequence = -1;
	enemiesHitThisAttack.clear();
	bossDefeatedEvent = false;
	spawnFromMap(levelMap);
}

void EnemyManager::clear() {
	enemies.clear();
	nextEnemyId = 0;
	activeAttackSequence = -1;
	enemiesHitThisAttack.clear();
	bossDefeatedEvent = false;
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
			auto enemy = std::make_unique<Enemy>(templateIt->second, sf::Vector2f(tileBounds.left, tileBounds.top));
			enemy->setId(nextEnemyId++);
			enemy->setLevelMap(&levelMap);
			enemies.push_back(std::move(enemy));
		}
	}
}

void EnemyManager::resolvePlayerAttack(const AttackInfo& attack) {
	// If attack is inactive, reset tracking
	if (!attack.active) {
		activeAttackSequence = -1;
		enemiesHitThisAttack.clear();
		return;
	}

	// If this is a new attack sequence, reset who we've hit
	if (attack.sequence != activeAttackSequence) {
		activeAttackSequence = attack.sequence;
		enemiesHitThisAttack.clear();
	}

	// Try to hit each alive enemy
	for (auto& enemy : enemies) {
		if (enemy && enemy->isAlive()) {
			tryHitEnemy(enemy.get(), attack);
		}
	}
}

bool EnemyManager::tryHitEnemy(Enemy* enemy, const AttackInfo& attack) {
	if (!enemy) {
		return false;
	}

	const int enemyId = enemy->getId();
	
	// Skip if already hit in this sequence
	if (enemiesHitThisAttack.count(enemyId) > 0) {
		return false;
	}

	// Check hitbox intersection
	if (!attack.hitbox.intersects(enemy->getDamageBounds())) {
		return false;
	}

	// Apply damage and feedback
	enemy->takeDamage(attack.damage);
	enemy->onHit(attack.damage, attack.direction);
	
	// Mark as hit so we don't hit it again this sequence
	enemiesHitThisAttack.insert(enemyId);
	
	return true;
}

void EnemyManager::update(float dt, map& levelMap, Player& player) {
	const sf::FloatRect playerBounds = player.getBounds();
	const sf::Vector2f playerTargetPosition = playerBounds.getPosition();
	for (auto& enemy : enemies) {
		enemy->setLevelMap(&levelMap);
		enemy->setTargetPlayerPosition(&playerTargetPosition);
		enemy->setTargetPlayerBounds(&playerBounds);
		enemy->update(dt);

		if (!enemy || !enemy->isAlive() || enemy->getDamage() <= 0) {
			continue;
		}

		const sf::FloatRect enemyBody = enemy->getBounds();
		const sf::Vector2f enemyCenter = {
			enemyBody.left + enemyBody.width * 0.5f,
			enemyBody.top + enemyBody.height * 0.5f
		};
		const sf::Vector2f playerCenter = {
			playerBounds.left + playerBounds.width * 0.5f,
			playerBounds.top + playerBounds.height * 0.5f
		};
		const HitboxDirection hitDirection = enemyCenter.x >= playerCenter.x ? HitboxDirection::Right : HitboxDirection::Left;

		if (enemy->usesFrameBasedPunchAttack()) {
			const sf::FloatRect punchHitbox = enemy->getAttackHitbox();

			if (enemy->isAttackDamageFrameActive() &&
				!enemy->hasAppliedAttackDamageThisSwing() &&
				punchHitbox.intersects(playerBounds)) {
				player.takeDamage(enemy->getDamage(), hitDirection);
				enemy->markAttackDamageApplied();
			}
			continue;
		}

		if (enemyBody.intersects(playerBounds)) {
			player.takeDamage(enemy->getDamage(), hitDirection);
		}
	}

	for (auto& enemy : enemies) {
		if (enemy && !enemy->isAlive() && enemy->animFinished()) {
			if (enemy->usesFrameBasedPunchAttack()) {
				bossDefeatedEvent = true;
			}
			// increment the players karma for each enemy killed
			player.setKarma(20);
		}
	}
	enemies.erase(
		std::remove_if(enemies.begin(), enemies.end(), [](const std::unique_ptr<Enemy>& enemy) {
			return !enemy->isAlive() && enemy->animFinished();
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

bool EnemyManager::consumeBossDefeatedEvent() {
	const bool defeated = bossDefeatedEvent;
	bossDefeatedEvent = false;
	return defeated;
}
