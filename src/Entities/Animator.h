



#ifndef PLATFORMER_ANIMATOR_H
#define PLATFORMER_ANIMATOR_H
#include <SFML/Graphics.hpp>
#include <iostream>
#include <map>
#include "AnimationSpec.h"

enum class AnimState {
	Loop,
	NonLoop,
	None
};

struct Animation {
	std::vector<sf::IntRect> frames;
	float frameDuration;
};

class Animator {
public:
	Animator();
	~Animator();


	void loadAnimations(const std::map<std::string, AnimationSpec>& animations);
	void addAnimation(const std::string& name, const std::vector<sf::IntRect>& frames, float frameDuration);
	bool hasAnimation(const std::string& name) const { return animations.find(name) != animations.end(); }
	void playAnimation(const std::string& name, bool loop = true);
	bool isNonLoopEnded() const { return nonLopingAnimEnded;}
	int getCurrentFrameIndex() const { return currentFrameIndex; }
	const std::string& getCurrentAnimationName() const { return currentAnimation; }
	void ChangeState(AnimState State);
	void stopAnimation();
	void update(float dt);
	void setFrame();

	AnimState getState() const {return animState;}


	void render(sf::RenderTarget& target);
	void setPosition(const float x,const float y){sprite.setPosition(x,y);};
	void setScale(const float x,const float y){sprite.setScale(x,y);}
	void setOrigin(const float x,const float y){sprite.setOrigin(x,y);}
	void setFlipX(bool flipped);
	sf::FloatRect getBounds() const;
	bool hasAnimations() const { return !animations.empty(); }

private:
	AnimState animState;
	std::map<std::string, Animation> animations;
	std::map<std::string, sf::Texture> textures;
	std::string currentAnimation = "idle";
	float elapsedTime;
	int currentFrameIndex;
	bool isPlaying;
	bool loop;
	bool nonLopingAnimEnded{true};
	bool flipX = false;
	sf::Sprite sprite;
};



#endif //PLATFORMER_ANIMATOR_H
