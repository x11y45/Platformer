



#ifndef PLATFORMER_PAUSEMENU_H
#define PLATFORMER_PAUSEMENU_H

#include <SFML/Graphics.hpp>
#include <array>
#include "MenuButton.h"

enum class PauseMenuAction {
	None,
	Resume,
	Restart,
	MainMenu
};

class PauseMenu {
public:
	PauseMenu(sf::Vector2u screenSize);
	void update(float dt, const sf::Vector2f& worldMousePos);
	void render(sf::RenderWindow& window) const;
	PauseMenuAction handleInput(const sf::Vector2f& worldMousePos) const;

private:
	float screenX;
	float screenY;
	sf::RectangleShape overlay;
	sf::Texture frameTexture;
	sf::Texture buttonHolderTexture;
	sf::Texture buttonHoverTexture;
	sf::Texture resumeTexture;
	sf::Texture restartTexture;
	sf::Texture menuTexture;
	sf::Sprite frameSprite;
	std::array<MenuButton, 3> buttons;
};

#endif // PLATFORMER_PAUSEMENU_H
