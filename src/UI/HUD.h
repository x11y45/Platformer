#ifndef PLATFORMER_HUD_H
#define PLATFORMER_HUD_H

#include <SFML/Graphics.hpp>
#include <vector>
#include "MenuButton.h"
#include "Entities/Player.h"

enum class HUDAction {
	None,
	Pause
};

class HUD {
public:
	HUD();

	void update(float dt, const sf::Vector2f& worldMousePos);
	void render(sf::RenderTarget& target,const Player& player);
	HUDAction handleInput(const sf::Vector2f& worldMousePos) const;

private:
	void updateHealthBar(unsigned int health, unsigned int maxHealth);
	void updateKarmaBar(unsigned int karma, unsigned int maxKarma);
	void updateFrameAnimation(float dt);

	sf::Texture sheetTexture;
	std::vector<sf::Texture> healthBarTextures;
	std::vector<sf::Texture> karmaBarTextures;
	sf::Sprite frameSprite;
	sf::Sprite healthBarSprite;
	sf::Sprite karmaBarSprite;
	MenuButton pauseButton;
	sf::Texture pauseButtonTexture;
	sf::Texture buttonHolderTexture;
	sf::Texture buttonHoverTexture;

	float frameTimer{0.f};
	std::size_t frameIndex{0};
	unsigned int lastHealth{200};
	unsigned int lastMaxHealth{200};
};

#endif // PLATFORMER_HUD_H
