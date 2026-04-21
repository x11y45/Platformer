//
// Created by x11y45 on 4/5/26.
//

#include "Player.h"
#include "Levels/map.h"
#include <algorithm>
#include <iostream>
#include <cmath>

namespace {
	/**
	 * the constants for player's rendering and hitbox calculations.
	 * these are based on the original sprite dimensions and animator origin.
	 **/
	constexpr float kPlayerRenderScale = 2.f;
	constexpr float kSpriteOriginY = 96.f; // Matches Animator origin (0, 96)
	constexpr float kHitboxLocalOffsetX = 54.5f; // In unscaled sprite pixels
	constexpr float kHitboxLocalOffsetY = 52.f; // In unscaled sprite pixels
	constexpr float kHitboxLocalWidth = 20.f;   // In unscaled sprite pixels
	constexpr float kHitboxLocalHeight = 45.f;  // In unscaled sprite pixels
}

Player::Player():
	health(MAX_HEALTH),
	karma(0),
	speed(200.f),
	jumpStrength(450.f),
	position(300.f, 300.f),
	velocity(0.f, 0.f),
	hurtPushRemaining(0.f, 0.f),
	moveDirection(0.f, 0.f),
	isMoving(false),
	isGrounded(false),
	facingRight(true),
	Jump(false),
	doubleJump(false),
	isFalling(false),
	isCrouching(false),
	isHealing(false),
	deltaTime(0.f),
	attackPhase(AttackPhase::None),
	lifeState(LifeState::Alive),
	attackFacingRight(true),
	attackSequence(0),
	healingStartedEvents(0),
	hurtEvents(0),
	width(kHitboxLocalWidth * kPlayerRenderScale),
	height(kHitboxLocalHeight * kPlayerRenderScale),
	hitboxOffset(
		kHitboxLocalOffsetX * kPlayerRenderScale,
		(kHitboxLocalOffsetY - kSpriteOriginY) * kPlayerRenderScale
	),
	bounds(0.f, 0.f, width, height) {
	updateBounds();
}

void Player::handleInput(const sf::Event &event) {
	if (lifeState != LifeState::Alive) {
		return;
	}

	if (animator.getState() == AnimState::NonLoop && !isHealing) {
		return;
	}

	// Event-based input (for actions that should trigger once)
	if (event.type == sf::Event::KeyPressed) {
		switch (event.key.code) {
			case sf::Keyboard::A:
			case sf::Keyboard::Left:
				isMoving = true;
				facingRight = false;
				moveDirection.x = -1.f;
				break;
			case sf::Keyboard::D:
			case sf::Keyboard::Right:
				isMoving = true;
				facingRight = true;
				moveDirection.x = 1.f;
				break;
			case sf::Keyboard::Space:
			case sf::Keyboard::W:
			case sf::Keyboard::Up:
				if (isGrounded) {
					Jump = true;
					isGrounded = false;
				} else if (!doubleJump) {
					Jump = true;
					doubleJump = true;
				}
				break;
			case sf::Keyboard::S:
			case sf::Keyboard::Down:
				isCrouching = true;
				break;
			case sf::Keyboard::X:
			case sf::Keyboard::K:
				if (isHealing) {
					cancelHealing();
				}
				startAttack();
				break;
			case sf::Keyboard::C:
				startHealing();
				break;
			default:
				break;
		}
	} else if (event.type == sf::Event::KeyReleased) {
		switch (event.key.code) {
			case sf::Keyboard::A:
			case sf::Keyboard::Left:
				if (moveDirection.x < 0.f) {
					moveDirection.x = 0.f;
					isMoving = false;
				}
				break;
			case sf::Keyboard::D:
			case sf::Keyboard::Right:
				if (moveDirection.x > 0.f) {
					moveDirection.x = 0.f;
					isMoving = false;
				}
				break;
			case sf::Keyboard::S:
			case sf::Keyboard::Down:
				isCrouching = false;
				break;
			default:
				break;
		}
	}
}

void Player::init(const std::map<std::string, AnimationSpec>& animations, const std::string &playerName) {
	this->name = playerName;
	animator.loadAnimations(animations);
	animator.setScale(kPlayerRenderScale, kPlayerRenderScale);
	animator.setPosition(position.x, position.y);
}

void Player::load() {
}

void Player::render(sf::RenderTarget &target) {
	if (animator.hasAnimations()) {
		animator.render(target);
	}
}

void Player::update(float dt) {
	deltaTime = dt;

	if (lifeState == LifeState::Hurt || lifeState == LifeState::Dead) {
		velocity = {0.f, 0.f};
		updateDamageReaction(dt);
		updateBounds();
		return;
	}

	if (isHealing && (isMoving || Jump || attackPhase != AttackPhase::None)) {
		cancelHealing();
	}

	if (moveDirection.x != 0.f) {
		facingRight = moveDirection.x > 0.f;
	}

	if (isMoving) {
		if (isGrounded && attackPhase == AttackPhase::Active) {
			stopAttack();
			attackPhase = AttackPhase::PendingClear; // End attack if player starts moving on ground
		}
		velocity.x = moveDirection.x * speed;
	} else {
		velocity.x = 0.f;
	}

	if (Jump) {
		if (doubleJump) {
			velocity.y = -jumpStrength * 0.8f; // Slightly weaker double jump
		} else {
			velocity.y = -jumpStrength;
		}
		isGrounded = false;
		Jump = false;
	}
	if (isGrounded) {
		Jump = false;
		doubleJump = false;
	}
	if (isCrouching) {
		velocity.x = 0.f; // Can't move while crouching

	}

	position += velocity * deltaTime;
	updateBounds();
}

void Player::updateMovementsStates(const CollisionResult& collisionResult) {
	const bool crouching = isCrouching && isGrounded;
	const float crouchShift = crouching ? height / 3.f : 0.f;

	position.x = collisionResult.correctedPosition.x - hitboxOffset.x;
	position.y = collisionResult.correctedPosition.y - hitboxOffset.y - crouchShift;

	if (collisionResult.top) { // Collided with the top of a platform (standing on it)
		isGrounded = true;
		if (velocity.y > 0.f) {
			velocity.y = 0.f;
		}
	} else {
		isGrounded = false;
	}

	if (collisionResult.bottom && velocity.y < 0.f) { // Collided with the bottom of a platform (hitting head)
		velocity.y = 0.f;
	}
	if (collisionResult.left || collisionResult.right) {
		velocity.x = 0.f;
	}

	if (isGrounded) {
		Jump = false;
		doubleJump = false;
	}
	applyPhysics(deltaTime);
	updateBounds();
	updateAnimation(deltaTime);
}

void Player::applyPhysics(float dt) {
	// Apply gravity
	velocity.y += 980.f * dt;
	if (velocity.y > 600.f) velocity.y = 600.f; // Terminal velocity
}


void Player::updateAnimation(float dt) {
	animator.setFlipX(!facingRight);

	if (isHealing) {
		if (!healAnimationStarted) {
			if (!animator.hasAnimation("healing")) {
				cancelHealing();
				return;
			}
			animator.playAnimation("healing", false);
			healAnimationStarted = true;
		}

		animator.update(dt);
		if (animator.isNonLoopEnded()) {
			completeHealing();
			animator.setPosition(position.x, position.y);
			return;
		}
		animator.setPosition(position.x, position.y);
		return;
	}

	if (lifeState == LifeState::Hurt) {
		animator.update(dt);
		animator.setPosition(position.x, position.y);
		if ((!animator.hasAnimation("Hurt") || animator.isNonLoopEnded()) && hurtPushRemaining.x == 0.f && hurtPushRemaining.y == 0.f) {
			lifeState = LifeState::Alive;
		}
		return;
	}

	if (lifeState == LifeState::Dead) {
		animator.update(dt);
		animator.setPosition(position.x, position.y);
		return;
	}

	const auto chooseAnimation = [&]() -> std::pair<const char*, bool> {
		if (isCrouching && isGrounded) {
			if (attackPhase == AttackPhase::Active) {
				return {"CrouchAttack", false};
			}
			return {"Crouch", true};
		}

		if (attackPhase == AttackPhase::Active) {
			if (!isGrounded) {
				return {"JumpAttack", false};
			}
			return {"Attack", false};
		}

		if (!isGrounded) {
			if (velocity.y < 0) {
				return {"Jump", true};
			}
			return {"Fall", true};
		}

		if (isMoving && std::abs(velocity.x) > 0.1f) {
			return {"Run", true};
		}

		return {"Idle", true};
	};

	const auto [animationName, loop] = chooseAnimation();
	animator.playAnimation(animationName, loop);

	animator.update(dt);
	if (attackPhase == AttackPhase::Active && animator.isNonLoopEnded()) {
		attackPhase = AttackPhase::PendingClear;
	}

	// Position sprite
	animator.setPosition(position.x, position.y);
}

void Player::unload() {
}

void Player::takeDamage(int damage, HitboxDirection hitDirection) {
	if (damage <= 0 || lifeState != LifeState::Alive) {
		return;
	}

	if (isHealing) {
		cancelHealing();
	}

	if (damage >= static_cast<int>(health)) {
		health = 0;
		beginDeath();
		return;
	}

	health -= static_cast<unsigned int>(damage);
	beginHurt(hitDirection);
}

void Player::setPosition(float x, float y) {
	position = {x, y};
	updateBounds();
	animator.setPosition(position.x, position.y);
}

sf::FloatRect Player::getBounds() const {
	return bounds;
}

AttackInfo Player::getAttackInfo() const {
	AttackInfo info;
	info.active = attackPhase != AttackPhase::None;
	info.sequence = attackSequence;
	info.damage = BASE_ATTACK_DAMAGE;
	info.direction = getAttackDirection();
	info.hitbox = getAttackBounds();
	return info;
}

void Player::startAttack() {
	if (attackPhase != AttackPhase::None) {
		return;
	}
	attackPhase = AttackPhase::Active;
	attackFacingRight = facingRight;
	attackSequence++;
}
void Player::stopAttack() {
	if (attackPhase == AttackPhase::Active) {
		attackPhase = AttackPhase::PendingClear;
	}
}

void Player::beginHurt(HitboxDirection hitDirection) {
	attackPhase = AttackPhase::None;
	lifeState = LifeState::Hurt;
	hurtEvents++;
	isMoving = false;
	moveDirection = {0.f, 0.f};
	Jump = false;
	doubleJump = false;
	isCrouching = false;
	velocity = {0.f, 0.f};

	const float pushX = hitDirection == HitboxDirection::Right ? -HURT_PUSH_DISTANCE :
		hitDirection == HitboxDirection::Left ? HURT_PUSH_DISTANCE : 0.f;
	hurtPushRemaining = {pushX, -HURT_PUSH_DISTANCE};

	if (animator.hasAnimation("Hurt")) {
		animator.playAnimation("Hurt", false);
	}
}

void Player::beginDeath() {
	cancelHealing();
	attackPhase = AttackPhase::None;
	lifeState = LifeState::Dead;
	isMoving = false;
	moveDirection = {0.f, 0.f};
	Jump = false;
	doubleJump = false;
	isCrouching = false;
	velocity = {0.f, 0.f};
	hurtPushRemaining = {0.f, 0.f};

	if (animator.hasAnimation("Death")) {
		animator.playAnimation("Death", false);
	}
}

void Player::updateDamageReaction(float dt) {
	if (hurtPushRemaining.x != 0.f) {
		const float stepX = std::min(std::abs(hurtPushRemaining.x), HURT_PUSH_SPEED * dt);
		position.x += hurtPushRemaining.x > 0.f ? stepX : -stepX;
		hurtPushRemaining.x += hurtPushRemaining.x > 0.f ? -stepX : stepX;
	}

	if (hurtPushRemaining.y != 0.f) {
		const float stepY = std::min(std::abs(hurtPushRemaining.y), HURT_PUSH_SPEED * dt);
		position.y += hurtPushRemaining.y > 0.f ? stepY : -stepY;
		hurtPushRemaining.y += hurtPushRemaining.y > 0.f ? -stepY : stepY;
	}
}

HitboxDirection Player::getAttackDirection() const {
	if (attackPhase == AttackPhase::None) {
		return HitboxDirection::None;
	}
	return attackFacingRight ? HitboxDirection::Right : HitboxDirection::Left;
}

sf::FloatRect Player::getAttackBounds() const {
	// Attack hitbox extends outward from player based on facing direction
	const float attackWidth = bounds.width * ATTACK_RANGE_MULTIPLIER;
	const float attackHeight = bounds.height;
	
	if (attackFacingRight) {
		// Hitbox extends to the right
		return {bounds.left + bounds.width, bounds.top, attackWidth, attackHeight};
	} else {
		// Hitbox extends to the left
		return {bounds.left - attackWidth, bounds.top, attackWidth, attackHeight};
	}
}

void Player::finalizeAttackFrame() {
	if (attackPhase == AttackPhase::PendingClear) {
		attackPhase = AttackPhase::None;
	}
}

void Player::startHealing() {
	const unsigned int healCost = MAX_KARMA / 10u;
	if (lifeState != LifeState::Alive || isHealing || health >= MAX_HEALTH || karma < healCost) {
		return;
	}
	if (isMoving || Jump || attackPhase != AttackPhase::None) {
		return;
	}
	if (!animator.hasAnimation("healing")) {
		return;
	}

	isHealing = true;
	healAnimationStarted = false;
	healingStartedEvents++;
}

void Player::cancelHealing() {
	isHealing = false;
	healAnimationStarted = false;
}

void Player::completeHealing() {
	if (!isHealing) {
		return;
	}

	const unsigned int healCost = MAX_KARMA / 10u;
	const unsigned int healAmount = MAX_HEALTH / 5u;
	if (karma >= healCost) {
		karma -= healCost;
		health = std::min(health + healAmount, MAX_HEALTH);
	}

	cancelHealing();
}
