#pragma once
#include "Player.h"
#include "Ball.h"

class PhysicsEngine
{
	public:
		PhysicsEngine(std::vector<Player*>* players, std::vector<Element>* walls, Ball* ball);
		GLfloat distanceBetweenHitboxes(Element* e1, Element* e2);
		
		void run(GLfloat& deltaTime);

	private:
		std::vector<Player*>*	players = nullptr;
		std::vector<Element>*	walls	= nullptr;
		Ball*					ball	= nullptr;
};

