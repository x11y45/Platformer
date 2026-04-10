//
// Created by x11y45 on 4/2/26.
//

#ifndef PLATFORMER_PAUSEMENU_H
#define PLATFORMER_PAUSEMENU_H

#include <SFML/Graphics.hpp>

enum class PauseMenuAction {
	None,
	Resume,
	Restart,
	MainMenu
};

class PauseMenu {
public:
	PauseMenu(sf::Vector2u screenSize);
	void update(float dt);
	void render(sf::RenderWindow& window) const;
	PauseMenuAction handleInput(const sf::Vector2f& worldMousePos) const;

private:
	float screenX;
	float screenY;
	sf::RectangleShape overlay;
	sf::Texture resumeTexture;
	sf::Texture restartTexture;
	sf::Texture menuTexture;
	sf::Sprite resumeButton;
	sf::Sprite restartButton;
	sf::Sprite menuButton;
};

#endif // PLATFORMER_PAUSEMENU_H
