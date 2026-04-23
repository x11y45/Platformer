


#ifndef PLATFORMER_MAINMENU_H
#define PLATFORMER_MAINMENU_H
#include "SFML/Graphics.hpp"
#include "Levels/map.h"
#include "MenuButton.h"
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
