



#include "Animator.h"

Animator::Animator():
	 elapsedTime(0.f)
	, isPlaying(false)
	, loop(true)
	, currentFrameIndex(0)
	, animState(AnimState::None){
	sprite.setOrigin(0.f, 96.f);
}

Animator::~Animator() = default;

namespace {

std::string chooseDefaultAnimation(const std::map<std::string, Animation>& animations) {
	if (animations.find("Idle") != animations.end()) {
		return "Idle";
	}
	if (animations.find("Run") != animations.end()) {
		return "Run";
	}
	if (animations.find("Walk") != animations.end()) {
		return "Walk";
	}

	for (const auto& [name, animation] : animations) {
		(void)animation;
		if (name != "Death") {
			return name;
		}
	}

	return animations.empty() ? std::string{} : animations.begin()->first;
}

}

void Animator::loadAnimations(const std::map<std::string, AnimationSpec>& animationsData) {
	for (const auto& [name, animData] : animationsData) {
		const std::string& texturePath = animData.path;
		int frameCount = animData.frameCount;

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
		addAnimation(name, frames, animData.frameDuration);
	}

	const std::string defaultAnimation = chooseDefaultAnimation(animations);
	if (!defaultAnimation.empty()) {
		playAnimation(defaultAnimation);
	}
}

void Animator::addAnimation(const std::string& name, const std::vector<sf::IntRect>& frames, float frameDuration) {
	animations[name] = {frames, frameDuration};
}

void Animator::playAnimation(const std::string& name, bool loopAnim) {
	if (currentAnimation == name && isPlaying) return;

	if (animations.find(name) != animations.end() && textures.find(name) != textures.end()) {
		currentAnimation = name;
		this->loop = loopAnim;
		ChangeState(loop ? AnimState::Loop : AnimState::NonLoop);
		isPlaying = true;
		elapsedTime = 0.f;
		currentFrameIndex = 0;
		sprite.setTexture(textures[name]);
		setFrame();
	}
}

void Animator::stopAnimation() {
	ChangeState(AnimState::None);
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
					ChangeState(AnimState::None);
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

void Animator::ChangeState(const AnimState State) {
	if (animState == AnimState::NonLoop && State != AnimState::NonLoop ){nonLopingAnimEnded = true;}
	else {nonLopingAnimEnded = false;}
	animState = State;
}

void Animator::setFlipX(const bool flipped) {
	if (flipX == flipped) {
		return;
	}
	flipX = flipped;
	setFrame();
}

sf::FloatRect Animator::getBounds() const {
	return sprite.getGlobalBounds();
}
