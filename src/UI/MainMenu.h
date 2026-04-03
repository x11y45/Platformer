//
// Created by x11y45 on 4/2/26.
//
#ifndef PLATFORMER_MAINMENU_H
#define PLATFORMER_MAINMENU_H
#include "SFML/Graphics.hpp"


class MainMenu {
public:
	sf::Sprite backgroundSprite;
	sf::Sprite middleGroundSpriteA;
	sf::Sprite middleGroundSpriteB;
	sf::Sprite MenuButtonSprite;
	sf::Sprite PlayButtonSprite;
	sf::Sprite LevelButtonSprite;
private:
	const float screenX;
	const float screenY;
	sf::Texture background;
	sf::Texture middleGround;
	//Buttons
	sf::Texture menuButtonTexture;
	sf::Texture playButtonTexture;
	sf::Texture levelButtonTexture;

public:
	MainMenu(sf::Vector2u screenSize);
	void init();
	void update(float dt);
	void render(sf::RenderWindow& window);
};



#endif //PLATFORMER_MAINMENU_H
