



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


	void resolvePlayerAttack(const AttackInfo& attack);

	void update(float dt, map& levelMap, Player& player);
	void render(sf::RenderTarget& target);
	std::size_t getEnemyCount() const;
	bool consumeBossDefeatedEvent();

private:
	void spawnFromMap(map& levelMap);


	bool tryHitEnemy(Enemy* enemy, const AttackInfo& attack);

	std::map<std::string, EnemyTemplate> templates;
	std::map<int, std::string> spawnMarkers;
	std::vector<std::unique_ptr<Enemy>> enemies;
	int nextEnemyId{0};
	int activeAttackSequence{-1};
	std::unordered_set<int> enemiesHitThisAttack;
	bool bossDefeatedEvent{false};
};

#endif //PLATFORMER_ENEMYMANAGER_H
