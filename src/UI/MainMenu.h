//
// Created by x11y45 on 4/2/26.
//
#ifndef PLATFORMER_MAINMENU_H
#define PLATFORMER_MAINMENU_H
#include "SFML/Graphics.hpp"
#include "Levels/map.h"
#include <array>
#include <vector>

enum class MainMenuAction {
	None,
	StartGame,
	OpenLevelMenu,
	Loading
};

class MainMenu {
private:
	struct MenuButton {
		MainMenuAction action{MainMenuAction::None};
		sf::Sprite holder;
		sf::Sprite hoverFill;
		sf::Sprite label;
		sf::Vector2f basePosition;
		float baseLabelScale{1.f};
		float baseHolderScaleX{1.f};
		float baseHolderScaleY{1.f};
		bool interactive{false};
		bool hovered{false};
	};

	const float screenX;
	const float screenY;
	bool loadingFinished{false};
	float loadingTimer{0.f};
	std::size_t loadingFrameIndex{0};
	sf::Texture buttonHolderTexture;
	sf::Texture buttonHoverTexture;
	sf::Texture menuButtonTexture;
	sf::Texture playButtonTexture;
	sf::Texture levelButtonTexture;
	std::array<MenuButton, 3> menuButtons;
	std::vector<sf::Texture> loadingTextures;
	sf::Sprite loadingSprite;
	std::vector<ParallaxLayer> parallaxLayers;

	void refreshButtonState(const sf::Vector2f& worldMousePos);

public:
	MainMenu(sf::Vector2u screenSize);
	void init(map &map);
	void update(float dt, const sf::Vector2f& worldMousePos);
	void render(sf::RenderTarget& target) const;
	MainMenuAction handleInput(const sf::Vector2f& worldMousePos) const;
};



#endif //PLATFORMER_MAINMENU_H
