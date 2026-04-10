//
// Created by x11y45 on 4/2/26.
//

#include "LevelMenu.h"
#include "Levels/LevelManager.hpp"
#include <algorithm>

LevelMenu::LevelMenu(sf::Vector2u screenSize)
	: screenX(static_cast<float>(screenSize.x))
	, screenY(static_cast<float>(screenSize.y)) {}

void LevelMenu::init(const LevelManager& levelManager) {
	levelIds = levelManager.getRegisteredLevelIds();
	levelButtons.clear();

	levelButtonTexture.loadFromFile("assets/level_button.png");
	backButtonTexture.loadFromFile("assets/menu_button.png");

	backButtonSprite.setTexture(backButtonTexture);
	backButtonSprite.setOrigin(backButtonSprite.getGlobalBounds().width / 2.f,
	                           backButtonSprite.getGlobalBounds().height / 2.f);
	backButtonSprite.setScale(2.5f, 2.5f);
	backButtonSprite.setPosition(screenX / 2.f, screenY * 0.2f);

	const float startY = screenY * 0.35f;
	const float spacing = 90.f;

	for (std::size_t i = 0; i < levelIds.size(); ++i) {
		sf::Sprite button;
		button.setTexture(levelButtonTexture);
		button.setOrigin(button.getGlobalBounds().width / 2.f, button.getGlobalBounds().height / 2.f);
		button.setScale(2.5f, 2.5f);
		button.setPosition(screenX / 2.f, startY + static_cast<float>(i) * spacing);
		levelButtons.push_back(button);
	}
}

void LevelMenu::update(float) {}

void LevelMenu::render(sf::RenderWindow& window) const {
	if (backButtonTexture.getSize().x > 0 && backButtonTexture.getSize().y > 0) {
		window.draw(backButtonSprite);
	}
	for (const auto& button : levelButtons) {
		window.draw(button);
	}
}

LevelMenuAction LevelMenu::handleInput(const sf::Vector2f& worldMousePos) const {
	if (backButtonSprite.getGlobalBounds().contains(worldMousePos)) {
		return {LevelMenuActionType::Back, -1};
	}
	for (std::size_t i = 0; i < levelButtons.size(); ++i) {
		if (levelButtons[i].getGlobalBounds().contains(worldMousePos)) {
			return {LevelMenuActionType::SelectLevel, levelIds[i]};
		}
	}
	return {};
}
