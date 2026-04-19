//
// Created by x11y45 on 4/2/26.
//

#ifndef PLATFORMER_LEVELMENU_H
#define PLATFORMER_LEVELMENU_H

#include <vector>
#include <SFML/Graphics.hpp>
#include "MenuButton.h"

class LevelManager;

enum class LevelMenuActionType {
	None,
	Back,
	SelectLevel
};

struct LevelMenuAction {
	LevelMenuActionType type{LevelMenuActionType::None};
	int levelId{-1};
};

class LevelMenu {
public:
	LevelMenu(sf::Vector2u screenSize);
	void init(const LevelManager& levelManager);
	void update(float dt, const sf::Vector2f& worldMousePos);
	void render(sf::RenderWindow& window) const;
	LevelMenuAction handleInput(const sf::Vector2f& worldMousePos) const;

private:
	float screenX;
	float screenY;
	sf::RectangleShape overlay;
	sf::Texture frameTexture;
	sf::Texture buttonHolderTexture;
	sf::Texture buttonHoverTexture;
	sf::Texture levelButtonTexture;
	sf::Texture backButtonTexture;
	sf::Sprite frameSprite;
	std::vector<int> levelIds;
	std::vector<MenuButton> buttons;
};

#endif // PLATFORMER_LEVELMENU_H
