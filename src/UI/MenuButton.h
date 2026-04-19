#ifndef PLATFORMER_MENUBUTTON_H
#define PLATFORMER_MENUBUTTON_H

#include <SFML/Graphics.hpp>

struct MenuButton {
	int actionId{-1};
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

#endif // PLATFORMER_MENUBUTTON_H
