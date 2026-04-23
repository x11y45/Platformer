



#ifndef PLATFORMER_COLLISION_H
#define PLATFORMER_COLLISION_H

#include <SFML/Graphics.hpp>
#include "../Levels/map.h"

struct CollisionResult {
	bool collided{false};
	bool top{false};
	bool bottom{false};
	bool left{false};
	bool right{false};
	bool isHarmful{false};
	sf::Vector2f correction{0.f, 0.f};
	sf::Vector2f correctedPosition{0.f, 0.f};

};

class Collision {
public:
	CollisionResult resolveMapCollision(const sf::FloatRect& bounds, map& levelMap) const;

private:
	static bool resolveOverlap(
		sf::FloatRect& resolvedBounds,
		const sf::FloatRect& tileBounds,
		CollisionResult& collisionResult
	);
};

#endif //PLATFORMER_COLLISION_H
