



#include "PauseMenu.h"
#include "MenuButtonStyle.h"

namespace {
	constexpr const char* kFramePath = "assets/DarkRedUISheet_crops/ui_panel_square_outline_a.png";
	constexpr const char* kButtonHolderPath = "assets/DarkRedUISheet_crops/ui_pill_button_outline_large.png";
	constexpr const char* kButtonHoverPath = "assets/DarkRedUISheet_crops/ui_pill_button_fill_large.png";
	constexpr const char* kResumePath = "assets/Headings/resume.png";
	constexpr const char* kRestartPath = "assets/Headings/restart.png";
	constexpr const char* kQuitPath = "assets/Headings/quit.png";
}

PauseMenu::PauseMenu(sf::Vector2u screenSize)
	: screenX(static_cast<float>(screenSize.x))
	, screenY(static_cast<float>(screenSize.y)) {
	overlay.setSize({screenX, screenY});
	overlay.setFillColor(sf::Color(0, 0, 0, 145));

	frameTexture.loadFromFile(kFramePath);
	buttonHolderTexture.loadFromFile(kButtonHolderPath);
	buttonHoverTexture.loadFromFile(kButtonHoverPath);
	resumeTexture.loadFromFile(kResumePath);
	restartTexture.loadFromFile(kRestartPath);
	menuTexture.loadFromFile(kQuitPath);

	MenuButtonStyle::centerSprite(frameSprite, frameTexture);
	frameSprite.setScale(7.2f, 4.5f);
	frameSprite.setPosition(screenX / 2.f, screenY * 0.53f);

	MenuButtonStyle::configure(buttons[0], resumeTexture, buttonHolderTexture, buttonHoverTexture,
	                           static_cast<int>(PauseMenuAction::Resume),
	                           {screenX / 2.f, screenY * 0.42f}, 2.05f, 38.f, 18.f, true);
	MenuButtonStyle::configure(buttons[1], restartTexture, buttonHolderTexture, buttonHoverTexture,
	                           static_cast<int>(PauseMenuAction::Restart),
	                           {screenX / 2.f, screenY * 0.54f}, 2.00f, 38.f, 18.f, true);
	MenuButtonStyle::configure(buttons[2], menuTexture, buttonHolderTexture, buttonHoverTexture,
	                           static_cast<int>(PauseMenuAction::MainMenu),
	                           {screenX / 2.f, screenY * 0.66f}, 1.95f, 38.f, 18.f, true);
}

void PauseMenu::update(float, const sf::Vector2f& worldMousePos) {
	for (auto& button : buttons) {
		MenuButtonStyle::update(button, worldMousePos);
	}
}

void PauseMenu::render(sf::RenderWindow& window) const {
	window.draw(overlay);
	window.draw(frameSprite);
	for (const auto& button : buttons) {
		window.draw(button.holder);
		if (button.hovered) {
			window.draw(button.hoverFill);
		}
		window.draw(button.label);
	}
}

PauseMenuAction PauseMenu::handleInput(const sf::Vector2f& worldMousePos) const {
	for (const auto& button : buttons) {
		if (!button.label.getGlobalBounds().contains(worldMousePos)) {
			continue;
		}
		switch (button.actionId) {
			case static_cast<int>(PauseMenuAction::Resume):
				return PauseMenuAction::Resume;
			case static_cast<int>(PauseMenuAction::Restart):
				return PauseMenuAction::Restart;
			case static_cast<int>(PauseMenuAction::MainMenu):
				return PauseMenuAction::MainMenu;
			default:
				break;
		}
	}
	return PauseMenuAction::None;
}
