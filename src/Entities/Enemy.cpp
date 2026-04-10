//
// Created by x11y45 on 4/9/26.
//

#include "Enemy.h"
#include "Levels/map.h"
#include <cmath>

namespace {
	constexpr float kEnemyGravity = 980.f;
	constexpr float kEnemyTerminalVelocity = 600.f;
}

Enemy::Enemy()
	: position(0.f, 0.f)
	, velocity(0.f, 0.f)
	, targetPlayerPosition(nullptr)
	, levelMap(nullptr)
	, bounds(0.f, 0.f, 32.f, 32.f)
	, grounded(false)
	, facingRight(true)
	, alive(true) {
	fallbackShape.setFillColor(sf::Color(180, 70, 70, 220));
}

Enemy::Enemy(const EnemyTemplate& config, const sf::Vector2f& spawnPosition)
	: Enemy() {
	configure(config, spawnPosition);
	load();
}

void Enemy::configure(const EnemyTemplate& config, const sf::Vector2f& spawnPosition) {
	definition = config;
	position = spawnPosition;
	velocity = {0.f, 0.f};
	grounded = false;
	facingRight = true;
	alive = true;
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
	definition.health -= amount;
	if (definition.health <= 0) {
		alive = false;
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

void Enemy::update(float dt) {
	if (!alive) {
		return;
	}

	if (definition.canMove && targetPlayerPosition && std::abs(targetPlayerPosition->x - position.x) < 2000.f && std::abs(targetPlayerPosition->y - position.y) < 200.f) {
		const float direction = targetPlayerPosition->x >= position.x ? 1.f : -1.f;
		velocity.x = direction * definition.speed;
		facingRight = direction > 0.f;
	} else {
		// TODO: goes left to right and back if the player is too far or not set.
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
			velocity.x = !velocity.x;
			facingRight = !facingRight;
			velocity.x = facingRight ? definition.speed : -definition.speed;
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
