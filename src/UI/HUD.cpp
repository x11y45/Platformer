#include "HUD.h"
#include "MenuButtonStyle.h"
#include <algorithm>
#include <cmath>

namespace {
	constexpr const char* kHUDSheetPath = "assets/UI/HeartSpriteSheet.png";
	std::string kHUDBarFramePath (const int num, const std::string& bar) {
		return "assets/UI/"+ bar +"/" + std::to_string(num*10) + ".png";
	}
	const sf::IntRect kHUDFrames[] = {
		{0, 0, 90,28},
		{90, 0, 90, 28}
	};
	const std::string health = "health";
	const std::string karma = "karma";
	constexpr float kFrameDuration = 0.18f;
	constexpr float kHUDPosX = 16.f;
	constexpr float kHUDPosY = 16.f;
	constexpr float kHealthBarPosX = 49.f;
	constexpr float kHealthBarPosY = 12.f;
	constexpr float kKarmaBarPosX = 49.f;
	constexpr float kKarmaBarPosY = 30.f;
	constexpr float kBarWidth = 85.f;
	constexpr float kBarHeight = 22.f;
	constexpr float kBarInsetX = 3.f;
	constexpr float kBarInsetY = 5.f;
	constexpr const char* kPauseButtonPath = "assets/pause_button.png";
	constexpr const char* kButtonHolderPath = "assets/DarkRedUISheet_crops/ui_pill_button_outline_large.png";
	constexpr const char* kButtonHoverPath = "assets/DarkRedUISheet_crops/ui_pill_button_fill_large.png";
}

HUD::HUD() {
	sheetTexture.loadFromFile(kHUDSheetPath);
	frameSprite.setTexture(sheetTexture);
	frameSprite.setTextureRect(kHUDFrames[0]);
	frameSprite.setPosition(kHUDPosX, kHUDPosY);
	frameSprite.setScale(320.f/180,320.f/180);

	healthBarSprite.setPosition(
		kHUDPosX + kHealthBarPosX,
		kHUDPosY + kHealthBarPosY
	);
	karmaBarSprite.setPosition(
		kHUDPosX + kKarmaBarPosX,
		kHUDPosY + kKarmaBarPosY
	);

	healthBarTextures.resize(11);
	karmaBarTextures.resize(11);

	for (int i = 0; i <= 10; ++i) {
		sf::Texture healthFrameTexture;
		sf::Texture karmaFrameTexture;
		karmaFrameTexture.loadFromFile(kHUDBarFramePath(i , karma ));
		healthFrameTexture.loadFromFile(kHUDBarFramePath(i , health ));
		healthBarTextures[i] = healthFrameTexture;
		karmaBarTextures[i] = karmaFrameTexture;
	}
	healthBarSprite.setTexture(healthBarTextures[10]);
	karmaBarSprite.setTexture(karmaBarTextures[10]);

	barFill.setFillColor(sf::Color(214, 69, 84));
	barFill.setPosition(
		kHUDPosX + kHealthBarPosX + kBarInsetX,
		kHUDPosY + kHealthBarPosY + kBarInsetY
	);
	barFill.setSize({kBarWidth - kBarInsetX * 2.f, kBarHeight - kBarInsetY * 2.f});

	pauseButtonTexture.loadFromFile(kPauseButtonPath);
	buttonHolderTexture.loadFromFile(kButtonHolderPath);
	buttonHoverTexture.loadFromFile(kButtonHoverPath);
	MenuButtonStyle::configure(pauseButton, pauseButtonTexture, buttonHolderTexture, buttonHoverTexture,
	                           static_cast<int>(HUDAction::Pause),
	                           {760.f, 44.f}, 1.4f, 24.f, 12.f, true);
}

void HUD::update(float dt, const sf::Vector2f& worldMousePos) {
	MenuButtonStyle::update(pauseButton, worldMousePos);
	updateFrameAnimation(dt);
}

void HUD::render(sf::RenderTarget& target, unsigned int health, unsigned int maxHealth) {
	updateHealthBar(health, maxHealth);
	target.draw(barFill);
	target.draw(frameSprite);
	target.draw(healthBarSprite);
	target.draw(karmaBarSprite);
	target.draw(pauseButton.holder);
	if (pauseButton.hovered) {
		target.draw(pauseButton.hoverFill);
	}
	target.draw(pauseButton.label);
}

HUDAction HUD::handleInput(const sf::Vector2f& worldMousePos) const {
	if (pauseButton.label.getGlobalBounds().contains(worldMousePos)) {
		return HUDAction::Pause;
	}
	return HUDAction::None;
}

void HUD::updateHealthBar(unsigned int health, unsigned int maxHealth) {
	lastHealth = health;
	lastMaxHealth = maxHealth == 0 ? 1 : maxHealth;

	const float healthPercent = static_cast<float>(lastHealth) / static_cast<float>(lastMaxHealth);
	const float snappedPercent = std::clamp(std::round(healthPercent * 10.f) / 10.f, 0.f, 1.f);
	healthBarSprite.setTexture(healthBarTextures[static_cast<int>(snappedPercent * 10.f)]);

	barFill.setSize({
		(kBarWidth - kBarInsetX * 2.f) * snappedPercent,
		kBarHeight - kBarInsetY * 2.f
	});
}

void HUD::updateFrameAnimation(float dt) {
	frameTimer += dt;
	if (frameTimer < kFrameDuration) {
		return;
	}

	frameTimer = 0.f;
	frameIndex = (frameIndex + 1) % 2;
	frameSprite.setTextureRect(kHUDFrames[frameIndex]);
}
