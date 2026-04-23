



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
	void startHealing();
	void cancelHealing();
	void completeHealing();


	AttackInfo getAttackInfo() const;


	void finalizeAttackFrame();



	sf::Vector2f getPosition() const { return position; }
	const sf::Vector2f& getPositionRef() const { return position; }
	void setPosition(float x, float y);
	sf::FloatRect getBounds() const;
	unsigned int getHealth() const { return health; }
	unsigned int getMaxHealth() const { return MAX_HEALTH; }
	unsigned int getMaxKarma() const { return MAX_KARMA; }
	unsigned int getKarma() const { return karma; }
	bool isDeathAnimationFinished() const { return lifeState == LifeState::Dead && (!animator.hasAnimation("Death") || animator.isNonLoopEnded()); }
	bool isAlive() const override { return lifeState != LifeState::Dead; }
	unsigned int getHealingStartedEvents() const { return healingStartedEvents; }
	unsigned int getHurtEvents() const { return hurtEvents; }


	void setHealth(unsigned int health) { this->health = std::min(this->health + health,MAX_HEALTH); }
	void setKarma(unsigned int karma) { this->karma = std::min(this->karma + karma, MAX_KARMA); }

private:
	void startAttack();
	void stopAttack();
	void beginHurt(HitboxDirection hitDirection);
	void beginDeath();
	void updateDamageReaction(float dt);


	sf::FloatRect getAttackBounds() const;


	HitboxDirection getAttackDirection() const;

	void updateAnimation(float dt);
	void applyPhysics(float dt);


	const unsigned int MAX_HEALTH = 200;
	const unsigned int MAX_KARMA = 200;
	std::string name;
	unsigned int health;
	unsigned int karma;
	float speed;
	float jumpStrength;


	constexpr static const float ATTACK_RANGE_MULTIPLIER = 2.0f;
	constexpr static const int BASE_ATTACK_DAMAGE = 1;
	constexpr static const float HURT_PUSH_DISTANCE = 40.f;
	constexpr static const float HURT_PUSH_SPEED = 220.f;


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
	bool isHealing;
	bool healAnimationStarted;
	bool attackFacingRight;
	int attackSequence;
	unsigned int healingStartedEvents;
	unsigned int hurtEvents;
	float deltaTime;


	float width;
	float height;
	sf::Vector2f hitboxOffset;
	sf::FloatRect bounds;

};

#endif //PLATFORMER_PLAYER_H
