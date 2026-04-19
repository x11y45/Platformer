#ifndef PLATFORMER_MENUBUTTONSTYLE_H
#define PLATFORMER_MENUBUTTONSTYLE_H

#include "MenuButton.h"

namespace MenuButtonStyle {
	struct Palette {
		sf::Color idleHolder{190, 120, 135, 195};
		sf::Color hoveredHolder{214, 138, 150, 225};
		sf::Color idleLabel{236, 208, 214, 255};
		sf::Color hoveredLabel{255, 232, 233, 255};
		sf::Color hoverFill{126, 71, 87, 170};
		float hoverScale{1.04f};
		float hoverLift{-2.f};
	};

	inline const Palette& defaultPalette() {
		static const Palette palette{};
		return palette;
	}

	inline void centerSprite(sf::Sprite& sprite, const sf::Texture& texture) {
		sprite.setTexture(texture);
		sprite.setOrigin(texture.getSize().x / 2.f, texture.getSize().y / 2.f);
	}

	inline void configure(MenuButton& button,
	                      const sf::Texture& labelTexture,
	                      const sf::Texture& holderTexture,
	                      const sf::Texture& hoverTexture,
	                      int actionId,
	                      const sf::Vector2f& position,
	                      float labelScale,
	                      float holderPaddingX,
	                      float holderPaddingY,
	                      bool interactive,
	                      const Palette& palette = defaultPalette()) {
		button.actionId = actionId;
		button.interactive = interactive;
		button.basePosition = position;
		button.baseLabelScale = labelScale;

		centerSprite(button.label, labelTexture);
		button.label.setScale(labelScale, labelScale);
		button.label.setPosition(position);

		centerSprite(button.holder, holderTexture);
		centerSprite(button.hoverFill, hoverTexture);
		const sf::FloatRect labelBounds = button.label.getGlobalBounds();
		button.baseHolderScaleX = (labelBounds.width + holderPaddingX) / static_cast<float>(holderTexture.getSize().x);
		button.baseHolderScaleY = (labelBounds.height + holderPaddingY) / static_cast<float>(holderTexture.getSize().y);
		button.holder.setScale(button.baseHolderScaleX, button.baseHolderScaleY);
		button.hoverFill.setScale(button.baseHolderScaleX, button.baseHolderScaleY);
		button.holder.setPosition(position);
		button.hoverFill.setPosition(position);
		button.holder.setColor(palette.idleHolder);
		button.hoverFill.setColor(sf::Color(255, 255, 255, 0));
		button.label.setColor(palette.idleLabel);
		button.hovered = false;
	}

	inline void update(MenuButton& button,
	                   const sf::Vector2f& worldMousePos,
	                   const Palette& palette = defaultPalette()) {
		button.hovered = button.interactive && button.label.getGlobalBounds().contains(worldMousePos);
		const float scaleBoost = button.hovered ? palette.hoverScale : 1.f;
		const float lift = button.hovered ? palette.hoverLift : 0.f;

		button.label.setScale(button.baseLabelScale * scaleBoost, button.baseLabelScale * scaleBoost);
		button.label.setPosition(button.basePosition.x, button.basePosition.y + lift);
		button.label.setColor(button.hovered ? palette.hoveredLabel : palette.idleLabel);

		button.holder.setScale(button.baseHolderScaleX * scaleBoost, button.baseHolderScaleY * scaleBoost);
		button.holder.setPosition(button.basePosition.x, button.basePosition.y + lift);
		button.holder.setColor(button.hovered ? palette.hoveredHolder : palette.idleHolder);

		button.hoverFill.setScale(button.baseHolderScaleX * scaleBoost, button.baseHolderScaleY * scaleBoost);
		button.hoverFill.setPosition(button.basePosition.x, button.basePosition.y + lift);
		button.hoverFill.setColor(button.hovered ? palette.hoverFill : sf::Color(255, 255, 255, 0));
	}
}

#endif // PLATFORMER_MENUBUTTONSTYLE_H
