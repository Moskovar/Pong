#pragma once
#include "Bone.h"
#include <map>
#include <vector>

class Animation
{
	public:
		Animation() {}
		Animation(std::string name, float duration, float ticksPerSecond, Bone* rootBone);

		//--- Getters ---//
		std::string getName()		{ return name;				}
		float getDuration()			{ return this->duration;	}
		float getTicksPerSecond()	{ return ticksPerSecond;	}
		Bone* getRootBone()			{ return rootBone;			}

		void animate(double animationTime);

	private:
		Bone* rootBone   = nullptr;
		std::string name = "";
		float duration   = 0, ticksPerSecond = 0;
};

