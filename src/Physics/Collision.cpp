



#include "Collision.h"
#include <cmath>

bool Collision::resolveOverlap(
	sf::FloatRect& resolvedBounds,
	const sf::FloatRect& tileBounds,
	CollisionResult& collisionResult
) {
	sf::FloatRect overlap;
	if (!resolvedBounds.intersects(tileBounds, overlap)) {
		return false;
	}
	const float playerCenterX = resolvedBounds.left + resolvedBounds.width * 0.5f;
	const float playerCenterY = resolvedBounds.top + resolvedBounds.height * 0.5f;
	const float tileCenterX = tileBounds.left + tileBounds.width * 0.5f;
	const float tileCenterY = tileBounds.top + tileBounds.height * 0.5f;

	if (overlap.width <= overlap.height) {
		const float correctionX = (playerCenterX < tileCenterX) ? -overlap.width : overlap.width;
		resolvedBounds.left += correctionX;
		collisionResult.left = collisionResult.left || (correctionX < 0.f);
		collisionResult.right = collisionResult.right || (correctionX > 0.f);
	} else {
		const float correctionY = (playerCenterY < tileCenterY) ? -overlap.height : overlap.height;
		resolvedBounds.top += correctionY;
		collisionResult.top = collisionResult.top || (correctionY < 0.f);
		collisionResult.bottom = collisionResult.bottom || (correctionY > 0.f);
	}

	collisionResult.collided = true;
	return true;
}

CollisionResult Collision::resolveMapCollision(const sf::FloatRect& bounds, map& levelMap) const {
	CollisionResult collisionResult;
	collisionResult.correctedPosition = {bounds.left, bounds.top};

	sf::FloatRect resolvedBounds = bounds;


	for (int pass = 0; pass < 3; ++pass) {
		bool resolvedAnyOverlap = false;
		const auto collidingTiles = levelMap.getCollidingTiles(resolvedBounds);

		for (const Tile* tile : collidingTiles) {
			if (!tile || tile->type == TileType::EMPTY) {
				continue;
			}
			const sf::FloatRect tileBounds = levelMap.getTileWorldBounds(*tile);
			resolvedAnyOverlap = resolveOverlap(resolvedBounds, tileBounds, collisionResult) || resolvedAnyOverlap;
		}
		if (!resolvedAnyOverlap) {
			break;
		}
	}

	collisionResult.correction = {
		resolvedBounds.left - bounds.left,
		resolvedBounds.top - bounds.top
	};
	collisionResult.correctedPosition = {resolvedBounds.left, resolvedBounds.top};
	return collisionResult;
}
