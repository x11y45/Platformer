//
// Created by x11y45 on 4/2/26.
//

#include "HUD.h"
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
}

HUD::HUD() {
	sheetTexture.loadFromFile(kHUDSheetPath);
	frameSprite.setTexture(sheetTexture);
	frameSprite.setTextureRect(kHUDFrames[0]);
	frameSprite.setPosition(kHUDPosX, kHUDPosY);
	frameSprite.setScale(320.f/180,320.f/180);

	// Load bar frame textures
	healthBarSprite.setPosition(
		kHUDPosX + kHealthBarPosX,
		kHUDPosY + kHealthBarPosY
	);
	karmaBarSprite.setPosition(
		kHUDPosX + kKarmaBarPosX,
		kHUDPosY + kKarmaBarPosY
	);


	healthBarTextures.resize(11);
	karmaBarTextures.resize(11); // 0% to 100% in 10% increments

	for (int i = 0; i <= 10; ++i) {
		sf::Texture healthFrameTexture;
		sf::Texture karmaFrameTexture;
		try {
			karmaFrameTexture.loadFromFile(kHUDBarFramePath(i , karma ));
			healthFrameTexture.loadFromFile(kHUDBarFramePath(i , health ));
		}		catch (const std::exception& err) {
			std::cerr << "Failed to load HUD bar frame texture for " << i*10
			          << "%: " << err.what() << std::endl;
		}
		healthBarTextures[i] = healthFrameTexture;
		karmaBarTextures[i] = karmaFrameTexture;
	}
	healthBarSprite.setTexture(healthBarTextures[10]); // Start with full bar frame.
	karmaBarSprite.setTexture(karmaBarTextures[10]); // Start with full bar frame.

	barFill.setFillColor(sf::Color(214, 69, 84));
	barFill.setPosition(
		kHUDPosX + kHealthBarPosX + kBarInsetX,
		kHUDPosY + kHealthBarPosY + kBarInsetY
	);
	barFill.setSize({kBarWidth - kBarInsetX * 2.f, kBarHeight - kBarInsetY * 2.f});
}

void HUD::update(float dt) {
	updateFrameAnimation(dt);
}

void HUD::render(sf::RenderTarget& target, unsigned int health, unsigned int maxHealth) {
	updateHealthBar(health, maxHealth);
	target.draw(barFill);
	target.draw(frameSprite);
	target.draw(healthBarSprite);
	target.draw(karmaBarSprite);
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
