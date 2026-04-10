//
// Created by x11y45 on 4/5/26.
//

#include "Player.h"
#include "Levels/map.h"
#include <iostream>
#include <cmath>

namespace {
	constexpr float kPlayerRenderScale = 2.f;
	constexpr float kSpriteOriginY = 96.f; // Matches Animator origin (0, 96)
	constexpr float kHitboxLocalOffsetX = 54.5f; // In unscaled sprite pixels
	constexpr float kHitboxLocalOffsetY = 52.f; // In unscaled sprite pixels
	constexpr float kHitboxLocalWidth = 20.f;   // In unscaled sprite pixels
	constexpr float kHitboxLocalHeight = 45.f;  // In unscaled sprite pixels
}

Player::Player():
	speed(200.f),
	jumpStrength(450.f),
	position(300.f, 300.f),
	velocity(0.f, 0.f),
	moveDirection(0.f, 0.f),
	isMoving(false),
	isGrounded(false),
	facingRight(true),
	Jump(false),
	doubleJump(false),
	isFalling(false),
	deltaTime(0.f),
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
					isGrounded = false;
					Jump = true;
				} else if (!doubleJump) {
					Jump = true;
					doubleJump = true;
				}
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
			default:
				break;
		}
	}
}

void Player::init(const std::map<std::string,std::pair<std::string,int>>& animations, const std::string &playerName) {
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

	isMoving = moveDirection.x != 0.f;
	if (moveDirection.x != 0.f) {
		facingRight = moveDirection.x > 0.f;
	}

	if (isMoving) {
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

	applyPhysics(dt);
	position += velocity * deltaTime;
	updateBounds();
}

void Player::updateMovementsStates(const CollisionResult& collisionResult) {

	position.x = collisionResult.correctedPosition.x - hitboxOffset.x;
	position.y = collisionResult.correctedPosition.y - hitboxOffset.y;

	if (collisionResult.top) { // Floor contact
		isGrounded = true;
		if (velocity.y > 0.f) {
			velocity.y = 0.f;
		}
	} else {
		isGrounded = false;
	}

	if (collisionResult.bottom && velocity.y < 0.f) { // Ceiling contact
		velocity.y = 0.f;
	}
	if (collisionResult.left || collisionResult.right) {
		velocity.x = 0.f;
	}

	if (isGrounded) {
		Jump = false;
		doubleJump = false;
	}

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

	// Choose animation based on state
	if (!isGrounded) {
		if (velocity.y < 0) {
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

	// Position sprite
	animator.setPosition(position.x, position.y);
}

void Player::unload() {
}

void Player::setPosition(float x, float y) {
	position = {x, y};
	updateBounds();
	animator.setPosition(position.x, position.y);
}

sf::FloatRect Player::getBounds() const {
	return bounds;
}
