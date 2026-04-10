//
// Created by x11y45 on 4/5/26.
//

#include "Animator.h"

Animator::Animator():
	 elapsedTime(0.f)
	, isPlaying(false)
	, loop(true)
	, currentFrameIndex(0){
	sprite.setOrigin(0.f, 96.f);
}

Animator::~Animator() = default;


void Animator::loadAnimations(const std::map<std::string,std::pair<std::string,int>>& animationsData) {
	for (const auto& [name, animData] : animationsData) {
		const std::string& texturePath = animData.first;
		int frameCount = animData.second;

		if (!textures[name].loadFromFile(texturePath)) {
			std::cerr << "Failed to load animation texture: " << texturePath << std::endl;
			continue;
		}

		std::vector<sf::IntRect> frames;
		int frameWidth = textures[name].getSize().x / frameCount;
		int frameHeight = textures[name].getSize().y;

		for (int i = 0; i < frameCount; ++i) {
			frames.emplace_back(i * frameWidth, 0, frameWidth, frameHeight);
		}
		addAnimation(name, frames, 0.12f); // Default frame duration
		
		// Set first animation as default
		if (currentAnimation.empty()) {
			sprite.setTexture(textures[name]);
			playAnimation(name);
		}
	}

	if (animations.find("Idle") != animations.end()) {
		playAnimation("Idle");
	} else if (!animations.empty()) {
		playAnimation(animations.begin()->first);
	}
}

void Animator::addAnimation(const std::string& name, const std::vector<sf::IntRect>& frames, float frameDuration) {
	animations[name] = {frames, frameDuration};
}

void Animator::playAnimation(const std::string& name, bool loopAnim) {
	if (currentAnimation == name && isPlaying) return; // Already playing
	
	if (animations.find(name) != animations.end() && textures.find(name) != textures.end()) {
		currentAnimation = name;
		this->loop = loopAnim;
		isPlaying = true;
		elapsedTime = 0.f;
		currentFrameIndex = 0;
		sprite.setTexture(textures[name]);
		setFrame();
	}
}
void Animator::stopAnimation() {
	isPlaying = false;
}
void Animator::update(float dt) {
	if (isPlaying && animations.find(currentAnimation) != animations.end()) {
		elapsedTime += dt;
		const Animation& anim = animations[currentAnimation];
		if (elapsedTime >= anim.frameDuration) {
			elapsedTime = 0.f;
			currentFrameIndex++;
			if (currentFrameIndex >= anim.frames.size()) {
				if (loop) {
					currentFrameIndex = 0;
				} else {
					currentFrameIndex = anim.frames.size() - 1;
					isPlaying = false;
				}
			}
			setFrame();
		}
	}
}

void Animator::setFrame() {
	if (animations.find(currentAnimation) != animations.end()) {
		const Animation& anim = animations[currentAnimation];
		if (!anim.frames.empty()) {
			sf::IntRect frame = anim.frames[currentFrameIndex];
			if (flipX) {
				frame.left += frame.width;
				frame.width = -frame.width;
			}
			sprite.setTextureRect(frame);
		}
	}
}

void Animator::render(sf::RenderTarget& target) {
	if (sprite.getTexture()) {
		target.draw(sprite);
	}
}

void Animator::setPosition(float x, float y) {
	sprite.setPosition(x, y);
}

void Animator::setScale(float x, float y) {
	sprite.setScale(x, y);
}
void Animator::setOrigin(float x, float y) {
	sprite.setOrigin(x, y);
}

void Animator::setFlipX(bool flipped) {
	if (flipX == flipped) {
		return;
	}

	flipX = flipped;
	setFrame();
}

sf::FloatRect Animator::getBounds() const {
	return sprite.getGlobalBounds();
}
