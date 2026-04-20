//
// Created by x11y45 on 4/9/26.
//

#include "Enemy.h"
#include "Levels/map.h"
#include <cmath>

namespace {
	/**
	 * this is the constants of the enemy
	 * the firs two are for physics, the @kEnemyDetectionRangeX/Y are for player tracking, if the player is within those ranges the enemy will start chasing him.
	 * the @kEnemyPatrolRangeX is how far the enemy will go from his home position before turning around, this is the behavior when the player is out of range.
	 */
	constexpr float kEnemyGravity = 1500.f;
	constexpr float kEnemyTerminalVelocity = 200.f;
	constexpr float kEnemyDetectionRangeX = 2000.f;
	constexpr float kEnemyDetectionRangeY = 200.f;
	constexpr float kEnemyPatrolRangeX = 120.f;
}

Enemy::Enemy()
	: position(0.f, 0.f)
	, velocity(0.f, 0.f)
	, hitPushRemaining(0.f, 0.f)
	, targetPlayerPosition(nullptr)
	, levelMap(nullptr)
	, bounds(0.f, 0.f, 32.f, 32.f)
	, grounded(false)
	, facingRight(true)
	, alive(true)
	, deathAnimationStarted(false)
	, deathAnimationFinished(false)
	, patrolDirection(1)
	, enemyId(-1) {
	fallbackShape.setFillColor(sf::Color(180, 70, 70, 220));
}

Enemy::Enemy(const EnemyTemplate& config, const sf::Vector2f& spawnPosition)
	: Enemy() {
	configure(config, spawnPosition);
	load();
}

void Enemy::configure(const EnemyTemplate& config, const sf::Vector2f& spawnPosition) {
	definition = config;
	homePosition = spawnPosition;
	position = spawnPosition;
	velocity = {0.f, 0.f};
	hitPushRemaining = {0.f, 0.f};
	grounded = false;
	facingRight = true;
	alive = true;
	deathAnimationStarted = false;
	deathAnimationFinished = false;
	patrolDirection = 1;
}

void Enemy::load() {
	if (!definition.animations.empty()) {
		animator.loadAnimations(definition.animations);
		animator.setOrigin(0.f, 0.f);
	}

	fallbackShape.setSize(definition.hitboxSize);
	fallbackShape.setFillColor(sf::Color(180, 70, 70, 220));
	updateBounds();
	syncVisuals();
}

void Enemy::unload() {
	alive = false;
}

void Enemy::handleInput(const sf::Event&) {
}

void Enemy::setLevelMap(map* mapPtr) {
	levelMap = mapPtr;
}

void Enemy::setTargetPlayerPosition(const sf::Vector2f* playerPosition) {
	targetPlayerPosition = playerPosition;
}

void Enemy::setId(int id) {
	enemyId = id;
}

int Enemy::getId() const {
	return enemyId;
}

void Enemy::setPosition(const sf::Vector2f& worldPosition) {
	position = worldPosition;
	updateBounds();
	syncVisuals();
}

sf::Vector2f Enemy::getPosition() const {
	return position;
}

sf::FloatRect Enemy::getBounds() const {
	return bounds;
}

int Enemy::getDamage() const {
	return definition.damage;
}

int Enemy::getHealth() const {
	return definition.health;
}

bool Enemy::isAlive() const {
	return alive;
}

void Enemy::takeDamage(int amount) {
	if (!alive) {
		return;
	}

	definition.health -= amount;
	if (definition.health <= 0) {
		alive = false;
		deathAnimationStarted = false;
		deathAnimationFinished = !animator.hasAnimation("Death");
	}
}

void Enemy::updatePatrolMovement() {
	const float patrolOffset = position.x - homePosition.x;
	if (patrolOffset >= kEnemyPatrolRangeX) {
		patrolDirection = -1;
	} else if (patrolOffset <= -kEnemyPatrolRangeX) {
		patrolDirection = 1;
	}

	velocity.x = static_cast<float>(patrolDirection) * definition.speed;
	facingRight = patrolDirection > 0;
}

void Enemy::onHit(int damage, HitboxDirection hitDirection) {
	(void)damage;
	// Only process feedback if cooldown has elapsed
	if (timeSinceLastHit > 0.f) {
		return;
	}

	// Apply knockback based on hit direction.
	// Push a short distance away from the player, then let normal AI resume.
	switch (hitDirection) {
		case HitboxDirection::Right:
			hitPushRemaining.x = HIT_PUSH_DISTANCE;
			break;
		case HitboxDirection::Left:
			hitPushRemaining.x = -HIT_PUSH_DISTANCE;
			break;
		case HitboxDirection::None:
			hitPushRemaining.x = 0.f;
			break;
	}

	hitPushRemaining.y = -HIT_PUSH_DISTANCE * 0.2f;

	// Start cooldown
	timeSinceLastHit = HIT_FEEDBACK_COOLDOWN;
}

void Enemy::updateHitPush(float dt) {
	if (hitPushRemaining.x != 0.f) {
		const float stepX = std::min(std::abs(hitPushRemaining.x), HIT_PUSH_SPEED * dt);
		position.x += hitPushRemaining.x > 0.f ? stepX : -stepX;
		hitPushRemaining.x += hitPushRemaining.x > 0.f ? -stepX : stepX;
	}

	if (hitPushRemaining.y != 0.f) {
		const float stepY = std::min(std::abs(hitPushRemaining.y), HIT_PUSH_SPEED * dt);
		position.y += hitPushRemaining.y > 0.f ? stepY : -stepY;
		hitPushRemaining.y += hitPushRemaining.y > 0.f ? -stepY : stepY;
	}
}

void Enemy::updateBounds() {
	bounds.left = position.x + definition.hitboxOffset.x;
	bounds.top = position.y + definition.hitboxOffset.y;
	bounds.width = definition.hitboxSize.x;
	bounds.height = definition.hitboxSize.y;
}

void Enemy::syncVisuals() {
	if (animator.hasAnimations()) {
		animator.setPosition(position.x, position.y);
		return;
	}

	fallbackShape.setPosition(bounds.left, bounds.top);
}

void Enemy::updateAnimation(float dt) {
	if (!animator.hasAnimations()) {
		return;
	}
	animator.setFlipX(!facingRight);
	if (!grounded) {
		if (velocity.y < 0.f) {
			animator.playAnimation("Jump");
		} else {
			animator.playAnimation("Fall");
		}
	} else if (std::abs(velocity.x) > 0.1f) {
		animator.playAnimation("Run");
	} else {
		animator.playAnimation("Idle");
	}
	animator.update(dt);
	animator.setPosition(position.x, position.y);
}

void Enemy::updateDeathAnimation(float dt) {
	if (!animator.hasAnimations()) {
		return;
	}

	if (!deathAnimationStarted) {
		if (animator.hasAnimation("Death")) {
			animator.playAnimation("Death", false);
		}
		deathAnimationStarted = true;
	}

	animator.update(dt);
	deathAnimationFinished = !animator.hasAnimation("Death") || animator.isNonLoopEnded();
	animator.setPosition(position.x, position.y);
}

void Enemy::update(float dt) {
	if (!alive) {
		updateDeathAnimation(dt);
		syncVisuals();
		return;
	}

	// Update hit feedback cooldown
	if (timeSinceLastHit > 0.f) {
		timeSinceLastHit -= dt;
	}

	if (hitPushRemaining.x != 0.f || hitPushRemaining.y != 0.f) {
		updateHitPush(dt);
		updateBounds();
		if (levelMap) {
			Collision collisionSystem;
			CollisionResult collisionResult = collisionSystem.resolveMapCollision(bounds, *levelMap);
			position.x = collisionResult.correctedPosition.x - definition.hitboxOffset.x;
			position.y = collisionResult.correctedPosition.y - definition.hitboxOffset.y;
			updateBounds();
		}
		syncVisuals();
		return;
	}

	const bool playerInRange = targetPlayerPosition &&
		std::abs(targetPlayerPosition->x - position.x) < kEnemyDetectionRangeX &&
		std::abs(targetPlayerPosition->y - position.y) < kEnemyDetectionRangeY;

	if (definition.canMove && playerInRange) {
		const float direction = targetPlayerPosition->x >= position.x ? 1.f : -1.f;
		velocity.x = direction * definition.speed;
		facingRight = direction > 0.f;
	} else if (definition.canMove) {
		updatePatrolMovement();
	} else {
		velocity.x = 0.f;
	}
	if (definition.canJump && grounded && targetPlayerPosition) {
		const float verticalGap = position.y - targetPlayerPosition->y;
		const float horizontalGap = std::abs(targetPlayerPosition->x - position.x);
		if (verticalGap > 8.f && horizontalGap < 96.f) {
			velocity.y = -definition.jumpStrength;
			grounded = false;
		}
	}

	velocity.y += kEnemyGravity * dt;
	if (velocity.y > kEnemyTerminalVelocity) {
		velocity.y = kEnemyTerminalVelocity;
	}

	position += velocity * dt;
	updateBounds();

	if (levelMap) {
		Collision collisionSystem;
		CollisionResult collisionResult = collisionSystem.resolveMapCollision(bounds, *levelMap);
		position.x = collisionResult.correctedPosition.x - definition.hitboxOffset.x;
		position.y = collisionResult.correctedPosition.y - definition.hitboxOffset.y;

		if (collisionResult.top && velocity.y > 0.f) {
			velocity.y = 0.f;
			grounded = true;
		} else if (!collisionResult.top) {
			grounded = false;
		}

		if (collisionResult.bottom && velocity.y < 0.f) {
			velocity.y = 0.f;
		}

		if (collisionResult.left || collisionResult.right) {
			patrolDirection *= -1;
			velocity.x = static_cast<float>(patrolDirection) * definition.speed;
			facingRight = patrolDirection > 0;
		}

		updateBounds();
	}

	updateAnimation(dt);
	syncVisuals();
}

void Enemy::render(sf::RenderTarget& target) {
	if (animator.hasAnimations()) {
		animator.render(target);
		return;
	}

	target.draw(fallbackShape);
}
