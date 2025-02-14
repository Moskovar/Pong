#include "Ball.h"

Ball::Ball(short id, glm::vec3 position, const std::string& filePath) : Element(id, position, filePath)
{
	this->moveSpeed = 25.0f;

	lastTimeDirectionChanged = getCurrentTimestampMilliseconds();

	//turn(90);
}

void Ball::move(GLfloat deltaTime)
{
	//translate(glm::vec3(direction * moveSpeed * deltaTime * velocityX, 0.0f, direction * moveSpeed * deltaTime * velocityZ));
	//updatePosition();

	position.x += direction * moveSpeed * deltaTime * velocityX;
	position.z += direction * moveSpeed * deltaTime * velocityZ;

	updateModelMatrixFromPosition();
}

void Ball::turnBack()
{
	//if ((getCurrentTimestampMilliseconds() - lastTimeDirectionChanged) <= 500) return;

	//turn(180);
	direction *= -1.0f;

	lastTimeDirectionChanged = getCurrentTimestampMilliseconds();
}

void Ball::update(NetworkBall& nball)
{
	this->position.x = (float)(nball.x / 1000.0f);
	this->position.z = (float)(nball.z / 1000.0f);

	updateModelMatrixFromPosition();

	this->velocityX = (float)(nball.velocityX / 1000.0f);
	this->velocityZ = (float)(nball.velocityZ / 1000.0f);

	std::cout << "BALL UPDATED: " << this->position.x << " : " << this->position.z << " : " << this->velocityX << " : " << this->velocityZ << std::endl;
}