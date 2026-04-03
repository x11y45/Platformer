//
// Created by x11y45 on 4/2/26.
//

#include "MainMenu.h"

MainMenu::MainMenu(const sf::Vector2u screenSize) : screenX(static_cast<float>(screenSize.x)), screenY(static_cast<float>(screenSize.y)) {
	init();
}

void MainMenu::init() {
	background.loadFromFile("assets/background.png");
	middleGround.loadFromFile("assets/middleground.png");

	const float scaleX = screenX / static_cast<float>(background.getSize().x);
	const float scaleY = screenY / static_cast<float>(background.getSize().y);
	backgroundSprite.setTexture(background);
	backgroundSprite.setPosition(0.f,0.f);
	backgroundSprite.setScale(scaleX,scaleY);
	middleGroundSpriteA.setTexture(middleGround);
	middleGroundSpriteA.setPosition(0.f,0.f);
	middleGroundSpriteA.setScale(scaleX,scaleY);
	middleGroundSpriteB.setTexture(middleGround);
	middleGroundSpriteB.setPosition(0.f + middleGroundSpriteA.getGlobalBounds().width,0.f);
	middleGroundSpriteB.setScale(scaleX,scaleY);

	menuButtonTexture.loadFromFile("assets/menu_button.png");
	MenuButtonSprite.setTexture(menuButtonTexture);
	MenuButtonSprite.setOrigin(MenuButtonSprite.getGlobalBounds().width / 2.f, MenuButtonSprite.getGlobalBounds().height / 2.f);
	MenuButtonSprite.setScale(5.f, 5.f);
	MenuButtonSprite.setPosition(screenX / 2.f, screenY / 3.f);

	playButtonTexture.loadFromFile("assets/play_button.png");
	PlayButtonSprite.setTexture(playButtonTexture);
	PlayButtonSprite.setOrigin(PlayButtonSprite.getGlobalBounds().width / 2.f, PlayButtonSprite.getGlobalBounds().height / 2.f);
	PlayButtonSprite.setScale(3.f, 3.f);
	PlayButtonSprite.setPosition(screenX / 2.f, screenY / 2.f);

	levelButtonTexture.loadFromFile("assets/level_button.png");
	LevelButtonSprite.setTexture(levelButtonTexture);
	LevelButtonSprite.setOrigin(LevelButtonSprite.getGlobalBounds().width / 2.f, LevelButtonSprite.getGlobalBounds().height / 2.f);
	LevelButtonSprite.setScale(3.f, 3.f);
	LevelButtonSprite.setPosition(screenX / 2.f, screenY * 2.f/ 3.f);
}

void MainMenu::update(const float dt) {
	float movement = 100.f * dt;
	const float width = middleGroundSpriteA.getGlobalBounds().width;
	middleGroundSpriteA.move(-movement,0);
	middleGroundSpriteB.move(-movement,0);
	if (middleGroundSpriteA.getPosition().x <= -width) {
		// Snap it to the right of Sprite B
		middleGroundSpriteA.setPosition(middleGroundSpriteB.getPosition().x + width, 0);
	}

	// 3. Check if Sprite B is off-screen
	if (middleGroundSpriteB.getPosition().x <= -width) {
		// Snap it to the right of Sprite A
		middleGroundSpriteB.setPosition(middleGroundSpriteA.getPosition().x + width, 0);
	}
}

void MainMenu::render(sf::RenderWindow& window) {
	window.draw(backgroundSprite);
	window.draw(middleGroundSpriteA);
	window.draw(middleGroundSpriteB);
	window.draw(MenuButtonSprite);
	window.draw(PlayButtonSprite);
	window.draw(LevelButtonSprite);
}