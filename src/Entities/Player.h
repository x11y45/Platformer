//
// Created by x11y45 on 4/5/26.
//

#ifndef PLATFORMER_PLAYER_H
#define PLATFORMER_PLAYER_H
#include "Entity.h"
#include "Combat.h"
#include "AnimationSpec.h"
#include "Physics/Collision.h"
#include <map>
#include <string>


class Player : public Entity {
public:
	Player();
	~Player() override = default;

	enum class AttackPhase {
		None,
		Active,
		PendingClear
	};

	enum class LifeState {
		Alive,
		Hurt,
		Dead
	};

	void init(const std::map<std::string, AnimationSpec>& animations, const std::string& name);
	void load() override;
	void unload() override;
	void update(float dt) override;
	void render(sf::RenderTarget& target) override;
	void handleInput(const sf::Event& event) override;

	void updateBounds() {
		const bool crouching = isCrouching && isGrounded;
		const float crouchShift = crouching ? height / 3.f : 0.f;
		const float crouchHeight = crouching ? (height * 2.f / 3.f) : height;
		bounds.left = position.x + hitboxOffset.x;
		bounds.top = position.y + hitboxOffset.y + crouchShift;
		bounds.width = width;
		bounds.height = crouchHeight;
	}
	void updateMovementsStates(const CollisionResult& collisionResult);
	void takeDamage(int damage, HitboxDirection hitDirection);
	
	/**
	 * Gets current attack state for EnemyManager to resolve hits
	 * @return AttackInfo with active state, sequence, damage, and directional hitbox
	 */
	AttackInfo getAttackInfo() const;
	
	/**
	 * Clears attack state after animation finishes
	 * Called by Level after attack resolution to ensure single-frame processing
	 */
	void finalizeAttackFrame();


	// Getters
	sf::Vector2f getPosition() const { return position; }
	const sf::Vector2f& getPositionRef() const { return position; }
	void setPosition(float x, float y);
	sf::FloatRect getBounds() const;
	unsigned int getHealth() const { return health; }
	unsigned int getMaxHealth() const { return MAX_HEALTH; }
	unsigned int getKarma() const { return karma; }
	bool isDeathAnimationFinished() const { return lifeState == LifeState::Dead && (!animator.hasAnimation("Death") || animator.isNonLoopEnded()); }
	bool isAlive() const override { return lifeState != LifeState::Dead; }

	// Setters
	void setHealth(unsigned int health) { this->health = health; }
	void setKarma(unsigned int karma) { this->karma = karma; }

private:
	void startAttack();
	void stopAttack();
	void beginHurt(HitboxDirection hitDirection);
	void beginDeath();
	void updateDamageReaction(float dt);
	
	/**
	 * Calculates attack hitbox based on player position and facing direction
	 * Hitbox extends in the direction player is facing, covering attack range
	 * @return World-space FloatRect for attack collision
	 */
	sf::FloatRect getAttackBounds() const;
	
	/**
	 * Determines attack direction based on current facing
	 * @return HitboxDirection for attack
	 */
	HitboxDirection getAttackDirection() const;
	
	void updateAnimation(float dt);
	void applyPhysics(float dt);
	
	// Player-specific attributes
	const unsigned int MAX_HEALTH = 200;
	const unsigned int MAX_KARMA = 200;
	std::string name;
	unsigned int health;
	unsigned int karma;
	float speed;
	float jumpStrength;
	
	// Attack constants
	constexpr static const float ATTACK_RANGE_MULTIPLIER = 2.0f;  ///< Hitbox size multiplier
	constexpr static const int BASE_ATTACK_DAMAGE = 1;             ///< Base damage per hit
	constexpr static const float HURT_PUSH_DISTANCE = 40.f;
	constexpr static const float HURT_PUSH_SPEED = 220.f;
	
	// Physics
	sf::Vector2f position;
	sf::Vector2f velocity;
	sf::Vector2f hurtPushRemaining;
	sf::Vector2f moveDirection;
	bool isMoving;
	bool isGrounded;
	bool facingRight;
	bool Jump;
	bool doubleJump;
	bool isFalling;
	AttackPhase attackPhase;
	LifeState lifeState;
	bool isCrouching;
	bool attackFacingRight;
	int attackSequence;
	float deltaTime;
	
	// Fixed hitbox inside the full animation frame
	float width;
	float height;
	sf::Vector2f hitboxOffset; //
	sf::FloatRect bounds;

};

#endif //PLATFORMER_PLAYER_H
