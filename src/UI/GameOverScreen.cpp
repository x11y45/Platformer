#include "GameOverScreen.h"
#include "MenuButtonStyle.h"

namespace {
	constexpr const char* kFramePath = "assets/DarkRedUISheet_crops/ui_panel_square_outline_a.png";
	constexpr const char* kButtonHolderPath = "assets/DarkRedUISheet_crops/ui_pill_button_outline_large.png";
	constexpr const char* kButtonHoverPath = "assets/DarkRedUISheet_crops/ui_pill_button_fill_large.png";
	constexpr const char* kRetryPath = "assets/play_button.png";
	constexpr const char* kMenuPath = "assets/menu_button.png";
}

GameOverScreen::GameOverScreen(sf::Vector2u screenSize)
	: screenX(static_cast<float>(screenSize.x))
	, screenY(static_cast<float>(screenSize.y)) {
	overlay.setSize({screenX, screenY});
	overlay.setFillColor(sf::Color(0, 0, 0, 160));

	frameTexture.loadFromFile(kFramePath);
	buttonHolderTexture.loadFromFile(kButtonHolderPath);
	buttonHoverTexture.loadFromFile(kButtonHoverPath);
	retryTexture.loadFromFile(kRetryPath);
	menuTexture.loadFromFile(kMenuPath);

	MenuButtonStyle::centerSprite(frameSprite, frameTexture);
	frameSprite.setScale(7.2f, 4.5f);
	frameSprite.setPosition(screenX / 2.f, screenY * 0.53f);

	MenuButtonStyle::configure(buttons[0], retryTexture, buttonHolderTexture, buttonHoverTexture,
	                           static_cast<int>(GameOverScreenAction::Retry),
	                           {screenX / 2.f, screenY * 0.46f}, 2.05f, 38.f, 18.f, true);
	MenuButtonStyle::configure(buttons[1], menuTexture, buttonHolderTexture, buttonHoverTexture,
	                           static_cast<int>(GameOverScreenAction::MainMenu),
	                           {screenX / 2.f, screenY * 0.59f}, 1.95f, 38.f, 18.f, true);
}

void GameOverScreen::update(float, const sf::Vector2f& worldMousePos) {
	for (auto& button : buttons) {
		MenuButtonStyle::update(button, worldMousePos);
	}
}

void GameOverScreen::render(sf::RenderTarget& target) const {
	target.draw(overlay);
	target.draw(frameSprite);
	for (const auto& button : buttons) {
		target.draw(button.holder);
		if (button.hovered) {
			target.draw(button.hoverFill);
		}
		target.draw(button.label);
	}
}

GameOverScreenAction GameOverScreen::handleInput(const sf::Vector2f& worldMousePos) const {
	for (const auto& button : buttons) {
		if (!button.label.getGlobalBounds().contains(worldMousePos)) {
			continue;
		}
		switch (button.actionId) {
			case static_cast<int>(GameOverScreenAction::Retry):
				return GameOverScreenAction::Retry;
			case static_cast<int>(GameOverScreenAction::MainMenu):
				return GameOverScreenAction::MainMenu;
			default:
				break;
		}
	}
	return GameOverScreenAction::None;
}
