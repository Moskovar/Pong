#pragma once
#include "Element.h"

class Ball : public Element
{
	public:
		Ball(short id, glm::vec3 position, const std::string& filePath);

		GLfloat getDirection() { return direction; }
		GLfloat getVelocityX() { return velocityX; }
		GLfloat getVelocityZ() { return velocityZ; }
		Element* getLastElementHit() { return lastElementHit; }

		bool directionJustChanged() { std::cout << getCurrentTimestamp() - lastTimeDirectionChanged << std::endl; return (getCurrentTimestamp() - lastTimeDirectionChanged) <= 500; }

		void setVelocityX(GLfloat velocityX) { this->velocityX = velocityX; }
		void setVelocityZ(GLfloat velocityZ) { this->velocityZ = velocityZ; }
		void setLastElementHit(Element* e);

		void move(GLfloat deltaTime);
		void turnBack();
		void reset();

		void updateInterpolate(NetworkBall& nball);
		void update(NetworkBall& nball);

	private:
		short direction = 1.0f;//1 ou - 1 pour donner la direction
		GLfloat velocityX = 0.0f, velocityZ = 0.0f;

		Element* lastElementHit = nullptr;

		uint64_t lastTimeDirectionChanged = 0;
};

