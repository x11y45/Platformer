//
// Created by x11y45 on 4/10/26.
//

#ifndef PLATFORMER_ANIMATIONSPEC_H
#define PLATFORMER_ANIMATIONSPEC_H

#include <string>

struct AnimationSpec {
	std::string path;
	int frameCount{1};
	float frameDuration{0.12f};
};

#endif //PLATFORMER_ANIMATIONSPEC_H
