//
// Created by x11y45 on 4/2/26.
//

#include "MainMenu.h"
#include <algorithm>
#include <filesystem>

namespace {
	constexpr float kLoadingFrameDuration = 0.06f;
	constexpr const char* kHeadingMenuPath = "assets/Headings/0.png";
	constexpr const char* kHeadingPlayPath = "assets/Headings/1.png";
	constexpr const char* kHeadingLevelPath = "assets/Headings/2.png";
	constexpr const char* kMenuFramePath = "assets/DarkRedUISheet_crops/ui_panel_square_outline_a.png";
	constexpr const char* kButtonHolderPath = "assets/DarkRedUISheet_crops/ui_pill_button_outline_large.png";
	constexpr const char* kButtonHoverPath = "assets/DarkRedUISheet_crops/ui_pill_button_fill_large.png";
	constexpr const char* kLoadingDir = "assets/ LoadingScreen";

	int parseFrameIndex(const std::filesystem::path& path) {
		return std::stoi(path.stem().string());
	}

	void centerSprite(sf::Sprite& sprite, const sf::Texture& texture) {
		sprite.setTexture(texture);
		sprite.setOrigin(texture.getSize().x / 2.f, texture.getSize().y / 2.f);
	}
}

MainMenu::MainMenu(const sf::Vector2u screenSize)
	: screenX(static_cast<float>(screenSize.x))
	, screenY(static_cast<float>(screenSize.y)) {
}

void MainMenu::init(map &map) {
	loadingFinished = false;
	loadingTimer = 0.f;
	loadingFrameIndex = 0;
	loadingTextures.clear();

	parallaxLayers = map.getParallaxLayer();
	const float scaleX = screenX / static_cast<float>(parallaxLayers[0].spriteA.getTexture()->getSize().x);
	const float scaleY = screenY / static_cast<float>(parallaxLayers[0].spriteA.getTexture()->getSize().y);

	for (auto& layer: parallaxLayers) {
		layer.spriteA.setTexture(layer.texture);
		layer.spriteB.setTexture(layer.texture);
		layer.spriteA.setScale(scaleX,scaleY);
		layer.spriteB.setScale(scaleX,scaleY);
		layer.spriteA.setPosition(0.f,0.f);
		layer.spriteB.setPosition(layer.spriteA.getGlobalBounds().width,0.f);
	}

	buttonHolderTexture.loadFromFile(kButtonHolderPath);
	buttonHoverTexture.loadFromFile(kButtonHoverPath);
	menuButtonTexture.loadFromFile(kHeadingMenuPath);
	playButtonTexture.loadFromFile(kHeadingPlayPath);
	levelButtonTexture.loadFromFile(kHeadingLevelPath);

	auto configureButton = [this](MenuButton& button,
	                              sf::Texture& labelTexture,
	                              MainMenuAction action,
	                              const sf::Vector2f& position,
	                              float labelScale,
	                              float holderPaddingX,
	                              float holderPaddingY,
	                              bool interactive) {
		button.actionId = static_cast<int>(action);
		button.interactive = interactive;
		button.basePosition = position;
		button.baseLabelScale = labelScale;

		centerSprite(button.label, labelTexture);
		button.label.setScale(labelScale, labelScale);
		button.label.setPosition(position);

		centerSprite(button.holder, buttonHolderTexture);
		centerSprite(button.hoverFill, buttonHoverTexture);
		const sf::FloatRect labelBounds = button.label.getGlobalBounds();
		button.baseHolderScaleX = (labelBounds.width + holderPaddingX) / static_cast<float>(buttonHolderTexture.getSize().x);
		button.baseHolderScaleY = (labelBounds.height + holderPaddingY) / static_cast<float>(buttonHolderTexture.getSize().y);
		button.holder.setScale(button.baseHolderScaleX, button.baseHolderScaleY);
		button.hoverFill.setScale(button.baseHolderScaleX, button.baseHolderScaleY);
		button.holder.setPosition(position);
		button.hoverFill.setPosition(position);
		button.holder.setColor(sf::Color(190, 120, 135, 195));
		button.hoverFill.setColor(sf::Color(255, 255, 255, 0));
		button.hovered = false;
	};

	configureButton(menuButtons[0], menuButtonTexture, MainMenuAction::None,
		{screenX / 2.f, screenY * 0.18f}, 2.15f, 28.f, 14.f, false);
	configureButton(menuButtons[1], playButtonTexture, MainMenuAction::StartGame,
		{screenX / 2.f, screenY * 0.50f}, 2.20f, 40.f, 20.f, true);
	configureButton(menuButtons[2], levelButtonTexture, MainMenuAction::OpenLevelMenu,
		{screenX / 2.f, screenY * 0.61f}, 2.15f, 40.f, 20.f, true);



	std::vector<std::filesystem::path> loadingFiles;
	for (const auto& entry : std::filesystem::directory_iterator(kLoadingDir)) {
		if (entry.is_regular_file() && entry.path().extension() == ".png") {
			loadingFiles.push_back(entry.path());
		}
	}
	std::sort(loadingFiles.begin(), loadingFiles.end(), [](const auto& a, const auto& b) {
		return parseFrameIndex(a) < parseFrameIndex(b);
	});

	for (const auto& path : loadingFiles) {
		sf::Texture texture;
		if (texture.loadFromFile(path.string())) {
			loadingTextures.push_back(std::move(texture));
		}
	}

	if (!loadingTextures.empty()) {
		loadingSprite.setTexture(loadingTextures.front());
		const sf::Vector2u size = loadingTextures.front().getSize();
		loadingSprite.setScale(screenX / static_cast<float>(size.x), screenY / static_cast<float>(size.y));
		loadingSprite.setOrigin(size.x / 2.f, size.y / 2.f);
		loadingSprite.setPosition(screenX / 2.f, screenY / 2.f);
	}
}

void MainMenu::refreshButtonState(const sf::Vector2f& worldMousePos) {
	for (auto& button : menuButtons) {
		button.hovered = button.interactive && button.label.getGlobalBounds().contains(worldMousePos);
		const float scaleBoost = button.hovered ? 1.04f : 1.0f;
		const float lift = button.hovered ? -2.f : 0.f;

		button.label.setScale(button.baseLabelScale * scaleBoost, button.baseLabelScale * scaleBoost);
		button.label.setPosition(button.basePosition.x, button.basePosition.y + lift);

		button.holder.setScale(button.baseHolderScaleX * scaleBoost, button.baseHolderScaleY * scaleBoost);
		button.holder.setPosition(button.basePosition.x, button.basePosition.y + lift);

		button.hoverFill.setScale(button.baseHolderScaleX * scaleBoost, button.baseHolderScaleY * scaleBoost);
		button.hoverFill.setPosition(button.basePosition.x, button.basePosition.y + lift);
		button.hoverFill.setColor(button.hovered ? sf::Color(126, 71, 87, 170) : sf::Color(255, 255, 255, 0));
		button.holder.setColor(button.hovered ? sf::Color(214, 138, 150, 225) : sf::Color(190, 120, 135, 195));
		button.label.setColor(button.hovered ? sf::Color(255, 232, 233, 255) : sf::Color(236, 208, 214, 255));
	}
}

void MainMenu::update(const float dt, const sf::Vector2f& worldMousePos) {
	if (!loadingFinished && !loadingTextures.empty()) {
		loadingTimer += dt;
		if (loadingTimer >= kLoadingFrameDuration) {
			loadingTimer -= kLoadingFrameDuration;
			++loadingFrameIndex;
			if (loadingFrameIndex >= loadingTextures.size()) {
				loadingFinished = true;
			} else {
				loadingSprite.setTexture(loadingTextures[loadingFrameIndex], true);
				const sf::Vector2u size = loadingTextures[loadingFrameIndex].getSize();
				loadingSprite.setOrigin(size.x / 2.f, size.y / 2.f);
			}
		}
	}

	refreshButtonState(worldMousePos);

	const auto Dt = dt;
	for (auto& layer : parallaxLayers) {
		const float width = layer.spriteA.getGlobalBounds().width;
		const float move = layer.parallaxFactor * Dt * 100.f;
		layer.spriteA.move(-move,0);
		layer.spriteB.move(-move,0);
		if (layer.spriteA.getPosition().x <= -width) {
			layer.spriteA.setPosition(layer.spriteB.getPosition().x+width,0);
		}
		if (layer.spriteB.getPosition().x <= -width) {
			layer.spriteB.setPosition(layer.spriteA.getPosition().x+width,0);
		}
	}
}

void MainMenu::render(sf::RenderTarget& target) const {
	for (const auto& layer : parallaxLayers) {
		target.draw(layer.spriteA);
		target.draw(layer.spriteB);
	}
	if (!loadingFinished && !loadingTextures.empty()) {
		target.draw(loadingSprite);
		return;
	}

	for (const auto& button : menuButtons) {
		if (button.interactive && button.hovered) {
			target.draw(button.hoverFill);
		}
		target.draw(button.holder);
		target.draw(button.label);
	}
}

MainMenuAction MainMenu::handleInput(const sf::Vector2f& worldMousePos) const {
	if (!loadingFinished) {
		return MainMenuAction::Loading;
	}
	for (const auto& button : menuButtons) {
		if (button.interactive && button.label.getGlobalBounds().contains(worldMousePos)) {
			return static_cast<MainMenuAction>(button.actionId);
		}
	}
	return MainMenuAction::None;
}
