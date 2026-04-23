



#ifndef PLATFORMER_ENTITY_H
#define PLATFORMER_ENTITY_H
#include <SFML/Graphics.hpp>
#include <map>
#include "Animator.h"


class Entity {
public:
	virtual ~Entity() = default;
	Entity():
		animator(){
	}


	virtual void load() = 0;
	virtual void unload() = 0;


	virtual void update(float dt) = 0;
	virtual void render(sf::RenderTarget& target) = 0;
	virtual void handleInput(const sf::Event& event) = 0;


	virtual bool isAlive() const { return true; }
protected:
	Animator animator;
};



#endif //PLATFORMER_ENTITY_H
