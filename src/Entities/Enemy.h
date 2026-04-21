//
// Created by x11y45 on 4/9/26.
//

#ifndef PLATFORMER_ENEMY_H
#define PLATFORMER_ENEMY_H

#include "Entity.h"
#include "Combat.h"
#include "EnemyConfig.h"
#include "Physics/Collision.h"
#include <SFML/Graphics.hpp>

class map;

class Enemy : public Entity {
public:
	Enemy();
	explicit Enemy(const EnemyTemplate& config, const sf::Vector2f& spawnPosition);
	~Enemy() override = default;

	void configure(const EnemyTemplate& config, const sf::Vector2f& spawnPosition);
	void load() override;
	void unload() override;
	void update(float dt) override;
	void render(sf::RenderTarget& target) override;
	void handleInput(const sf::Event& event) override;

	void setLevelMap(map* map_ptr) { this->levelMap = map_ptr; }
	void setTargetPlayerPosition(const sf::Vector2f* playerPosition) { this->targetPlayerPosition = playerPosition; }
	void setTargetPlayerBounds(const sf::FloatRect* playerBounds) { this->targetPlayerBounds = playerBounds; }

	void setPosition(const sf::Vector2f& worldPosition);

	bool usesFrameBasedPunchAttack() const;
	bool isAttackDamageFrameActive() const;
	bool hasAppliedAttackDamageThisSwing() const;
	void markAttackDamageApplied();
	float getAttackRangeMultiplier() const;
	sf::FloatRect getAttackHitbox() const ;
	sf::FloatRect getDamageBounds() const;

	void setId(int id) { enemyId = id; }
	int getId() const { return enemyId; }
	sf::Vector2f getPosition() const { return position; }
	sf::FloatRect getBounds() const { return bounds; }
	int getDamage() const { return definition.damage; }
	int getHealth() const { return definition.health; }
	bool canAttack() const { return definition.canAttack; }
	bool isFacingRight() const { return facingRight; }
	bool isAlive() const override{ return alive; }
	bool isDying() const { return !alive && !deathAnimationFinished; }
	bool animFinished() const { return deathAnimationFinished || animator.isNonLoopEnded(); }
	
	/**
	 * Inflicts damage on the enemy
	 * @param amount Damage to take
	 */
	void takeDamage(int amount);
	/**
	 * Called when enemy is hit by a player attack
	 * Handles visual/audio feedback and applies knockback if applicable
	 * @param damage Damage taken in this hit
	 * @param hitDirection Direction the hit came from (for knockback/visual effects)
	 */
	void onHit(int damage, HitboxDirection hitDirection);

private:
	enum class EnemyActionState {
		Patrol,
		Hurt,
		Attack,
		Idle
	};

	void updateBounds();
	void updateAnimation(float dt);
	void updateDeathAnimation(float dt);
	void updatePatrolMovement();
	void updateHitPush(float dt);
	void resolveMapCollision();
	void applyConfiguredAnimationState();
	void applyFacingDirection();
	bool shouldStartAttack() const;
	void syncVisuals();
	
	// Hit feedback timing (prevents visual spam from multiple hits in same frame)
	float timeSinceLastHit{0.f};
	constexpr static float HIT_FEEDBACK_COOLDOWN = 1.f;

	// Attack constant
	constexpr static float	HIT_PUSH_DISTANCE = 50.f;
	constexpr static float HIT_PUSH_SPEED = 200.f;

	EnemyTemplate definition;
	int enemyId;
	sf::Vector2f homePosition;
	sf::Vector2f position;
	sf::Vector2f velocity;
	sf::Vector2f hitPushRemaining;
	int patrolDirection{1};
	EnemyActionState actionState{EnemyActionState::Patrol};
	bool actionAnimationStarted{false};
	bool attackDamageAppliedThisSwing{false};
	int attacksCompletedInBurst{0};
	float attackCooldownRemaining{0.f};
	float burstRestCooldownRemaining{0.f};
	const sf::Vector2f* targetPlayerPosition;
	const sf::FloatRect* targetPlayerBounds{nullptr};
	map* levelMap;
	sf::FloatRect bounds;
	sf::RectangleShape fallbackShape;
	bool grounded;
	bool facingRight;
	bool alive;
	bool deathAnimationStarted{false};
	bool deathAnimationFinished{false};
};

#endif //PLATFORMER_ENEMY_H
