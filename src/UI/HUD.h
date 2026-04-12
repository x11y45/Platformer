//
// Created by x11y45 on 4/2/26.
//

#ifndef PLATFORMER_HUD_H
#define PLATFORMER_HUD_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

class HUD {
public:
	HUD();

	void update(float dt);
	void render(sf::RenderTarget& target, unsigned int health, unsigned int maxHealth);

private:
	void updateHealthBar(unsigned int health, unsigned int maxHealth);
	void updateFrameAnimation(float dt);

	sf::Texture sheetTexture;
	std::vector<sf::Texture> healthBarTextures; // 0-10 health frames
	std::vector<sf::Texture> karmaBarTextures; // 0-10 karma frames
	sf::Sprite frameSprite;
	sf::Sprite healthBarSprite;
	sf::Sprite karmaBarSprite;
	sf::RectangleShape barFill;

	float frameTimer{0.f};
	std::size_t frameIndex{0};
	unsigned int lastHealth{200};
	unsigned int lastMaxHealth{200};
};

#endif //PLATFORMER_HUD_H
