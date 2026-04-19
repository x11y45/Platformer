#ifndef PLATFORMER_GAMEOVERSCREEN_H
#define PLATFORMER_GAMEOVERSCREEN_H

#include <SFML/Graphics.hpp>
#include <array>
#include "MenuButton.h"

enum class GameOverScreenAction {
	None,
	Retry,
	MainMenu
};

class GameOverScreen {
public:
	GameOverScreen(sf::Vector2u screenSize);
	void update(float dt, const sf::Vector2f& worldMousePos);
	void render(sf::RenderTarget& target) const;
	GameOverScreenAction handleInput(const sf::Vector2f& worldMousePos) const;

private:
	float screenX;
	float screenY;
	sf::RectangleShape overlay;
	sf::Texture frameTexture;
	sf::Texture buttonHolderTexture;
	sf::Texture buttonHoverTexture;
	sf::Texture retryTexture;
	sf::Texture menuTexture;
	sf::Sprite frameSprite;
	std::array<MenuButton, 2> buttons;
};

#endif // PLATFORMER_GAMEOVERSCREEN_H
