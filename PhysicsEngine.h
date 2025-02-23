#pragma once
#include "Player.h"
#include "Ball.h"
#include <mutex>
#include "SpellBox.h"

class PhysicsEngine
{
	public:
		PhysicsEngine(std::vector<Player*>* players, std::vector<Element>* walls, Ball* ball, SpellBox* spellBox, std::mutex* mtx_ball);
		GLfloat distanceBetweenHitboxes(Element* e1, Element* e2);
		
		void run(GLfloat& deltaTime);
		void runWarmup(GLfloat& deltaTime);

	private:
		std::vector<Player*>*	players		= nullptr;
		std::vector<Element>*	walls		= nullptr;
		Ball*					ball		= nullptr;
		SpellBox*				spellBox	= nullptr;
		std::mutex*				mtx_ball	= nullptr;
};

