//
// Created by x11y45 on 4/9/26.
//

#ifndef PLATFORMER_ENEMYMANAGER_H
#define PLATFORMER_ENEMYMANAGER_H

#include <memory>
#include <map>
#include <vector>
#include <unordered_set>
#include "Combat.h"
#include "Enemy.h"

class map;
class Player;

class EnemyManager {
public:
	void load(
		const std::map<std::string, EnemyTemplate>& templates,
		const std::map<int, std::string>& spawnMarkers,
		map& levelMap
	);
	void clear();
	
	/**
	 * Resolves player attack against all alive enemies
	 * 
	 * Each swing (sequence) can only hit each enemy once. When a new attack sequence
	 * begins, all enemies become hittable again. Hitbox intersections are checked
	 * each frame while attack is active.
	 * 
	 * @param attack AttackInfo from Player containing hitbox, direction, and sequence
	 */
	void resolvePlayerAttack(const AttackInfo& attack);
	
	void update(float dt, map& levelMap, Player& player);
	void render(sf::RenderTarget& target);
	std::size_t getEnemyCount() const;

private:
	void spawnFromMap(map& levelMap);
	
	/**
	 * Helper to check and apply hit to a single enemy
	 * @param enemy Enemy to potentially hit
	 * @param attack Attack information
	 * @return true if hit was applied (enemy wasn't already hit this sequence)
	 */
	bool tryHitEnemy(Enemy* enemy, const AttackInfo& attack);

	std::map<std::string, EnemyTemplate> templates;
	std::map<int, std::string> spawnMarkers;
	std::vector<std::unique_ptr<Enemy>> enemies;
	int nextEnemyId{0};
	int activeAttackSequence{-1};  ///< Current swing ID (prevents multiple hits per swing)
	std::unordered_set<int> enemiesHitThisAttack;  ///< Enemies already hit in current sequence
};

#endif //PLATFORMER_ENEMYMANAGER_H
