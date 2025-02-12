#include "Animation.h"

Animation::Animation(std::string name, float duration, float ticksPerSecond, Bone* rootBone)
{
	this->name			 = name;
	this->duration		 = duration;
	this->ticksPerSecond = ticksPerSecond;
	this->rootBone		 = rootBone;

	printf("Duration: %f, Ticks: %f\n", duration, ticksPerSecond);
}

void Animation::animate(double animationTime)
{
	glm::mat4 parentTransform(1.0f);
	//if(rootBone) rootBone->applyTransformations(rootBone->interpolateTransform(animationTime), parentTransform, animationTime);
}
