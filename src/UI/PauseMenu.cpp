//
// Created by x11y45 on 4/2/26.
//

#include "PauseMenu.h"

PauseMenu::PauseMenu(sf::Vector2u screenSize)
	: screenX(static_cast<float>(screenSize.x))
	, screenY(static_cast<float>(screenSize.y)) {
	overlay.setSize({screenX, screenY});
	overlay.setFillColor(sf::Color(0, 0, 0, 140));

	resumeTexture.loadFromFile("assets/play_button.png");
	restartTexture.loadFromFile("assets/pause_button.png");
	menuTexture.loadFromFile("assets/menu_button.png");

	resumeButton.setTexture(resumeTexture);
	resumeButton.setOrigin(resumeButton.getGlobalBounds().width / 2.f,
	                       resumeButton.getGlobalBounds().height / 2.f);
	resumeButton.setScale(2.5f, 2.5f);
	resumeButton.setPosition(screenX / 2.f, screenY * 0.35f);

	restartButton.setTexture(restartTexture);
	restartButton.setOrigin(restartButton.getGlobalBounds().width / 2.f,
	                        restartButton.getGlobalBounds().height / 2.f);
	restartButton.setScale(2.5f, 2.5f);
	restartButton.setPosition(screenX / 2.f, screenY * 0.5f);

	menuButton.setTexture(menuTexture);
	menuButton.setOrigin(menuButton.getGlobalBounds().width / 2.f,
	                    menuButton.getGlobalBounds().height / 2.f);
	menuButton.setScale(2.5f, 2.5f);
	menuButton.setPosition(screenX / 2.f, screenY * 0.65f);
}

void PauseMenu::update(float) {}

void PauseMenu::render(sf::RenderWindow& window) const {
	window.draw(overlay);
	window.draw(resumeButton);
	window.draw(restartButton);
	window.draw(menuButton);
}

PauseMenuAction PauseMenu::handleInput(const sf::Vector2f& worldMousePos) const {
	if (resumeButton.getGlobalBounds().contains(worldMousePos)) {
		return PauseMenuAction::Resume;
	}
	if (restartButton.getGlobalBounds().contains(worldMousePos)) {
		return PauseMenuAction::Restart;
	}
	if (menuButton.getGlobalBounds().contains(worldMousePos)) {
		return PauseMenuAction::MainMenu;
	}
	return PauseMenuAction::None;
}
