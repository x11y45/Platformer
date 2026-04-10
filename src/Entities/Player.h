//
// Created by x11y45 on 4/5/26.
//

#ifndef PLATFORMER_PLAYER_H
#define PLATFORMER_PLAYER_H
#include "Entity.h"
#include "Physics/Collision.h"
#include <map>
#include <string>

class map; // Forward declaration

class Player : public Entity {
public:
	Player();
	~Player() override = default;

	void init(const std::map<std::string,std::pair<std::string,int>>& animations, const std::string& name);
	void load() override;
	void unload() override;
	void update(float dt) override;
	void render(sf::RenderTarget& target) override;
	void handleInput(const sf::Event& event) override;

	void updateBounds() {
		bounds.left = position.x + hitboxOffset.x;
		bounds.top = position.y + hitboxOffset.y;
		bounds.width = width;
		bounds.height = height;
	}
	void updateMovementsStates(const CollisionResult& collisionResult);


	// Getters
	sf::Vector2f getPosition() const { return position; }
	const sf::Vector2f& getPositionRef() const { return position; }
	void setPosition(float x, float y);
	sf::FloatRect getBounds() const;

private:
	void updateAnimation(float dt);
	void applyPhysics(float dt);
	
	// Player-specific attributes
	std::string name;
	unsigned int health{};
	unsigned int karma{};
	float speed;
	float jumpStrength;
	
	// Physics
	sf::Vector2f position;
	sf::Vector2f velocity;
	sf::Vector2f moveDirection;
	bool isMoving;
	bool isGrounded;
	bool facingRight;
	bool Jump;
	bool doubleJump;
	bool isFalling;
	float deltaTime;
	
	// Fixed hitbox inside the full animation frame
	float width;
	float height;
	sf::Vector2f hitboxOffset; //
	sf::FloatRect bounds;

};

#endif //PLATFORMER_PLAYER_H
