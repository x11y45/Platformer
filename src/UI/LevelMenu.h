//
// Created by x11y45 on 4/2/26.
//

#ifndef PLATFORMER_LEVELMENU_H
#define PLATFORMER_LEVELMENU_H

#include <vector>
#include <SFML/Graphics.hpp>

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
	void update(float dt);
	void render(sf::RenderWindow& window) const;
	LevelMenuAction handleInput(const sf::Vector2f& worldMousePos) const;

private:
	float screenX;
	float screenY;
	sf::Texture levelButtonTexture;
	sf::Texture backButtonTexture;
	sf::Sprite backButtonSprite;
	std::vector<int> levelIds;
	std::vector<sf::Sprite> levelButtons;
};

#endif // PLATFORMER_LEVELMENU_H
