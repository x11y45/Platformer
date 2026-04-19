//
// Created by x11y45 on 4/2/26.
//

#include "LevelMenu.h"
#include "Levels/LevelManager.hpp"
#include "MenuButtonStyle.h"
#include <algorithm>

namespace {
	constexpr const char* kFramePath = "assets/DarkRedUISheet_crops/ui_panel_square_outline_a.png";
	constexpr const char* kButtonHolderPath = "assets/DarkRedUISheet_crops/ui_pill_button_outline_large.png";
	constexpr const char* kButtonHoverPath = "assets/DarkRedUISheet_crops/ui_pill_button_fill_large.png";
	constexpr const char* kLevelButtonPath = "assets/Headings/2.png";
	constexpr const char* kBackButtonPath = "assets/Headings/0.png";
}

LevelMenu::LevelMenu(sf::Vector2u screenSize)
	: screenX(static_cast<float>(screenSize.x))
	, screenY(static_cast<float>(screenSize.y)) {
	overlay.setSize({screenX, screenY});
	overlay.setFillColor(sf::Color(0, 0, 0, 115));
}

void LevelMenu::init(const LevelManager& levelManager) {
	levelIds = levelManager.getRegisteredLevelIds();
	buttons.clear();

	frameTexture.loadFromFile(kFramePath);
	buttonHolderTexture.loadFromFile(kButtonHolderPath);
	buttonHoverTexture.loadFromFile(kButtonHoverPath);
	levelButtonTexture.loadFromFile(kLevelButtonPath);
	backButtonTexture.loadFromFile(kBackButtonPath);

	MenuButtonStyle::centerSprite(frameSprite, frameTexture);
	frameSprite.setScale(7.25f, 4.6f);
	frameSprite.setPosition(screenX / 2.f, screenY * 0.54f);

	buttons.reserve(levelIds.size() + 1);
	MenuButton backButton;
	MenuButtonStyle::configure(backButton, backButtonTexture, buttonHolderTexture, buttonHoverTexture,
	                           static_cast<int>(LevelMenuActionType::Back),
	                           {screenX / 2.f, screenY * 0.23f}, 1.95f, 34.f, 18.f, true);
	buttons.push_back(backButton);

	const float startY = screenY * 0.42f;
	const float spacing = 82.f;
	for (std::size_t i = 0; i < levelIds.size(); ++i) {
		MenuButton button;
		MenuButtonStyle::configure(button, levelButtonTexture, buttonHolderTexture, buttonHoverTexture,
		                           levelIds[i],
		                           {screenX / 2.f, startY + static_cast<float>(i) * spacing}, 2.05f, 38.f, 18.f, true);
		buttons.push_back(button);
	}
}

void LevelMenu::update(float, const sf::Vector2f& worldMousePos) {
	for (auto& button : buttons) {
		MenuButtonStyle::update(button, worldMousePos);
	}
}

void LevelMenu::render(sf::RenderWindow& window) const {
	window.draw(overlay);
	window.draw(frameSprite);
	for (const auto& button : buttons) {
		window.draw(button.holder);
		if (button.interactive && button.hovered) {
			window.draw(button.hoverFill);
		}
		window.draw(button.label);
	}
}

LevelMenuAction LevelMenu::handleInput(const sf::Vector2f& worldMousePos) const {
	for (const auto& button : buttons) {
		if (!button.interactive || !button.label.getGlobalBounds().contains(worldMousePos)) {
			continue;
		}
		if (button.actionId == static_cast<int>(LevelMenuActionType::Back)) {
			return {LevelMenuActionType::Back, -1};
		}
		return {LevelMenuActionType::SelectLevel, button.actionId};
	}
	return {};
}
