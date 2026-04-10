//
// Created by x11y45 on 4/9/26.
//

#ifndef PLATFORMER_ENEMY_H
#define PLATFORMER_ENEMY_H

#include "Entity.h"
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

	void setLevelMap(map* levelMap);
	void setTargetPlayerPosition(const sf::Vector2f* playerPosition);
	void setPosition(const sf::Vector2f& worldPosition);
	sf::Vector2f getPosition() const;
	sf::FloatRect getBounds() const;
	int getDamage() const;
	int getHealth() const;
	bool isAlive() const override;
	void takeDamage(int amount);

private:
	void updateBounds();
	void updateAnimation(float dt);
	void syncVisuals();

	EnemyTemplate definition;
	sf::Vector2f position;
	sf::Vector2f velocity;
	const sf::Vector2f* targetPlayerPosition;
	map* levelMap;
	sf::FloatRect bounds;
	sf::RectangleShape fallbackShape;
	bool grounded;
	bool facingRight;
	bool alive;
};

#endif //PLATFORMER_ENEMY_H
