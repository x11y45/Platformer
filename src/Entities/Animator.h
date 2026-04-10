//
// Created by x11y45 on 4/5/26.
//

#ifndef PLATFORMER_ANIMATOR_H
#define PLATFORMER_ANIMATOR_H
#include <SFML/Graphics.hpp>
#include <iostream>
#include <map>

struct Animation {
	std::vector<sf::IntRect> frames;
	float frameDuration;
};

class Animator {
public:
	Animator();
	~Animator();

	// Animation management
	void loadAnimations(const std::map<std::string,std::pair<std::string,int>>& animations);
	void addAnimation(const std::string& name, const std::vector<sf::IntRect>& frames, float frameDuration);
	void playAnimation(const std::string& name, bool loop = true);
	void stopAnimation();
	void update(float dt);
	void setFrame();
	
	// Rendering
	void render(sf::RenderTarget& target);
	void setPosition(float x, float y);
	void setScale(float x, float y);
	void setOrigin(float x, float y);
	void setFlipX(bool flipped);
	sf::FloatRect getBounds() const;
	bool hasAnimations() const { return !animations.empty(); }
	
private:
	std::map<std::string, Animation> animations;
	std::map<std::string, sf::Texture> textures;
	std::string currentAnimation = "idle";
	float elapsedTime;
	int currentFrameIndex;
	bool isPlaying;
	bool loop;
	bool flipX = false;
	sf::Sprite sprite;
};



#endif //PLATFORMER_ANIMATOR_H
