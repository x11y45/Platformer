//
// Created by x11y45 on 4/10/26.
//

#ifndef PLATFORMER_COMBAT_H
#define PLATFORMER_COMBAT_H

#include <SFML/Graphics.hpp>

/**
 * @enum HitboxDirection
 * Direction of attack hitbox extension from player
 */
enum class HitboxDirection {
	Left,   ///< Hitbox extends left
	Right,  ///< Hitbox extends right
	None    ///< No direction (inactive)
};

/**
 * @struct AttackInfo
 * Information about an active player attack
 * Used by EnemyManager to resolve hits once per swing
 */
struct AttackInfo {
	bool active{false};              ///< Whether an attack is currently active
	int sequence{0};                 ///< Unique ID for this swing (incremented each attack)
	int damage{1};                   ///< Damage dealt to enemies
	HitboxDirection direction{HitboxDirection::None};  ///< Direction hitbox extends
	sf::FloatRect hitbox{};          ///< World-space hitbox for collision
};

/**
 * @struct HitResult
 * Result of an enemy being hit by an attack
 */
struct HitResult {
	bool hit{false};                 ///< Whether the hit was successful
	int damageDealt{0};              ///< Actual damage dealt to enemy
	HitboxDirection hitDirection{HitboxDirection::None};  ///< Direction of the hit
};

#endif //PLATFORMER_COMBAT_H
