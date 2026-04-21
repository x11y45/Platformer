//
// Created by x11y45 on 4/9/26.
//

#ifndef PLATFORMER_ENEMYCONFIG_H
#define PLATFORMER_ENEMYCONFIG_H

#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include "AnimationSpec.h"

struct EnemyTemplate {
	std::string name;
	std::map<std::string, AnimationSpec> animations;
	int damage{1};
	float speed{0.f};
	int health{1};
	bool canMove{true};
	bool canJump{false};
	bool canAttack{false};
	float jumpStrength{0.f};
	sf::Vector2f hitboxSize{32.f, 32.f};
	sf::Vector2f hitboxOffset{0.f, 0.f};
	sf::Vector2f attackHitboxOffset{0.f, 0.f};
	sf::Vector2f attackHitboxSize{0.f, 0.f};
	bool hasAttackHitbox{false};
	float renderScale{1.f};
	bool flipWhenFacingRight{false};
};

#endif //PLATFORMER_ENEMYCONFIG_H
