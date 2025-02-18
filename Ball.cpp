#include "Ball.h"

Ball::Ball(short id, glm::vec3 position, const std::string& filePath) : Element(id, position, filePath)
{
	//this->moveSpeed = 50.0f;
}

void Ball::setLastElementHit(Element* e)
{
	this->lastElementHit = e;
}

void Ball::move(GLfloat deltaTime)
{
	//translate(glm::vec3(direction * moveSpeed * deltaTime * velocityX, 0.0f, direction * moveSpeed * deltaTime * velocityZ));
	//updatePosition();
	//std::cout << "VelocityX: " << velocityX << std::endl;
	//std::cout << moveSpeed << std::endl;
	position.x += moveSpeed * deltaTime * velocityX;
	position.z += moveSpeed * deltaTime * velocityZ;

	updateModelMatrixFromPosition();
}

void Ball::turnBack()//useless?
{
	velocityX *= -1;
}

void Ball::updateInterpolate(NetworkBall& nball)
{
	glm::vec3 targetBallPos = glm::vec3((float)(nball.x / 1000.0f), 0.0f, (float)(nball.z / 1000.0f));
	float interpolationFactor = 0.016f; // Ajustable selon les besoins

	//this->position.x = (float)(nball.x / 1000.0f);
	//this->position.z = (float)(nball.z / 1000.0f);

	this->position = glm::mix(this->position, targetBallPos, interpolationFactor);

	updateModelMatrixFromPosition();

	this->velocityX = (float)(nball.velocityX / 1000.0f);
	this->velocityZ = (float)(nball.velocityZ / 1000.0f);

	//std::cout << "BALL UPDATED: " << this->position.x << " : " << this->position.z << " : " << this->velocityX << " : " << this->velocityZ << std::endl;
}

void Ball::update(NetworkBall& nball)
{
	this->position.x = (float)(nball.x / 1000.0f);
	this->position.z = (float)(nball.z / 1000.0f);

	updateModelMatrixFromPosition();

	this->velocityX = (float)(nball.velocityX / 1000.0f);
	this->velocityZ = (float)(nball.velocityZ / 1000.0f);

	//std::cout << "BALL UPDATED: " << this->position.x << " : " << this->position.z << " : " << this->velocityX << " : " << this->velocityZ << std::endl;
}

//glm::vec3 currentBallPos; // Position actuelle affichée
//glm::vec3 targetBallPos;  // Dernière position reçue du serveur
//float interpolationFactor = 0.1f; // Ajustable selon les besoins
//
//void updateBallPosition() {
//	// Lissage de la position pour éviter les téléportations
//	currentBallPos = glm::mix(currentBallPos, targetBallPos, interpolationFactor);
//}
