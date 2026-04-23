



#ifndef PLATFORMER_COMBAT_H
#define PLATFORMER_COMBAT_H

#include <SFML/Graphics.hpp>


enum class HitboxDirection {
	Left,
	Right,
	None
};


struct AttackInfo {
	bool active{false};
	int sequence{0};
	int damage{1};
	HitboxDirection direction{HitboxDirection::None};
	sf::FloatRect hitbox{};
};


struct HitResult {
	bool hit{false};
	int damageDealt{0};
	HitboxDirection hitDirection{HitboxDirection::None};
};

#endif //PLATFORMER_COMBAT_H
