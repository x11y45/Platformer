//
// Created by x11y45 on 4/9/26.
//

#ifndef PLATFORMER_ENEMYCONFIG_H
#define PLATFORMER_ENEMYCONFIG_H

#include <SFML/Graphics.hpp>
#include <map>
#include <string>

struct EnemyTemplate {
	std::string name;
	std::map<std::string, std::pair<std::string, int>> animations;
	int damage{1};
	float speed{0.f};
	int health{1};
	bool canMove{true};
	bool canJump{false};
	bool canAttack{false};
	float jumpStrength{0.f};
	sf::Vector2f hitboxSize{32.f, 32.f};
	sf::Vector2f hitboxOffset{0.f, 0.f};
};

#endif //PLATFORMER_ENEMYCONFIG_H
