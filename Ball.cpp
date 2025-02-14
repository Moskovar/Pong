#include "Ball.h"

Ball::Ball(short id, glm::vec3 position, const std::string& filePath) : Element(id, position, filePath)
{
	this->moveSpeed = 50.0f;
}

void Ball::move(GLfloat deltaTime)
{
	//translate(glm::vec3(direction * moveSpeed * deltaTime * velocityX, 0.0f, direction * moveSpeed * deltaTime * velocityZ));
	//updatePosition();
	std::cout << "VelocityX: " << velocityX << std::endl;

	position.x += moveSpeed * deltaTime * velocityX;
	position.z += moveSpeed * deltaTime * velocityZ;

	updateModelMatrixFromPosition();
}

void Ball::turnBack()//useless?
{
	direction *= -1.0f;
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