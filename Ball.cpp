#include "Ball.h"

Ball::Ball(short id, glm::vec3 position, const std::string& filePath) : Element(id, position, filePath)
{
	//this->moveSpeed = 50.0f;
}

void Ball::setLastElementHit(Element* e)
{
	this->lastElementHit = e;
}

void Ball::updateInterpolateFactors(NetworkBall& nb)
{
    //lastVelocityX = velocityX;
    //lastVelocityZ = velocityZ;

    // Calculer les nouvelles vitesses
    this->velocityX = (float)(nb.velocityX / 1000.0f);
    this->velocityZ = (float)(nb.velocityZ / 1000.0f);
    this->moveSpeed = nb.speed;

    //// Détection du changement de direction
    //if (lastVelocityX <= 0 && velocityX > 0 || lastVelocityX >= 0 && velocityX < 0) interpolateFactorX = 1.0f;
    //if (lastVelocityZ <= 0 && velocityZ > 0 || lastVelocityZ >= 0 && velocityZ < 0) interpolateFactorZ = 1.0f;

    //// Ajuster progressivement le facteur d'interpolation X
    //float targetInterpolateFactorX = 1.0f;  // Valeur cible du facteur d'interpolation X
    //if (velocityX < 0)
    //{
    //    if (nb.x < position.x) targetInterpolateFactorX = 1.05f; // Viser une légère augmentation
    //    else if (nb.x > position.x) targetInterpolateFactorX = 0.95f; // Viser une légère diminution
    //}
    //else if (velocityX > 0)
    //{
    //    if (nb.x < position.x) targetInterpolateFactorX = 0.95f;
    //    else if (nb.x > position.x) targetInterpolateFactorX = 1.05f;
    //}

    //// Interpolation exponentielle pour limiter l'impact
    //interpolateFactorX = glm::mix(interpolateFactorX, targetInterpolateFactorX, 0.05f); // Lissage doux

    //// Ajuster progressivement le facteur d'interpolation Z
    //float targetInterpolateFactorZ = 1.0f;  // Valeur cible du facteur d'interpolation Z
    //if (velocityZ < 0)
    //{
    //    if (nb.z < position.z) targetInterpolateFactorZ = 1.05f;
    //    else if (nb.z > position.z) targetInterpolateFactorZ = 0.95f;
    //}
    //else if (velocityZ > 0)
    //{
    //    if (nb.z < position.z) targetInterpolateFactorZ = 0.95f;
    //    else if (nb.z > position.z) targetInterpolateFactorZ = 1.05f;
    //}

    //// Interpolation exponentielle pour limiter l'impact
    //interpolateFactorZ = glm::mix(interpolateFactorZ, targetInterpolateFactorZ, 0.05f); // Lissage doux

    //// Limiter les facteurs d'interpolation dans une plage raisonnable
    //interpolateFactorX = glm::clamp(interpolateFactorX, 0.85f, 1.15f); // Plage plus douce
    //interpolateFactorZ = glm::clamp(interpolateFactorZ, 0.85f, 1.15f);

    glm::vec3 velocityDirection = glm::normalize(glm::vec3(velocityX, 0.0f, velocityZ));

    // Calculer la différence sur les axes X et Z
    float diffX = nb.x - position.x;
    float diffZ = nb.z - position.z;

    std::cout << diffX << " : " << diffZ << std::endl;
}

void Ball::move(GLfloat deltaTime)
{
    position.x += moveSpeed * deltaTime * velocityX;// *interpolateFactorX;
    position.z += moveSpeed * deltaTime * velocityZ;// *interpolateFactorZ;

	//if(interpolateFactorX != 1.0f || interpolateFactorZ != 1.0f) std::cout << interpolateFactorX << " : " << interpolateFactorZ << std::endl;

	updateModelMatrixFromPosition();
}

void Ball::turnBack()//useless?
{
	velocityX *= -1;
}

void Ball::reset()
{
	this->position	= glm::vec3(0.0f, 0.0f, 0.0f);
	this->velocityX = 0.0f;
	this->velocityZ = 0.0f;
	this->moveSpeed = 50;
}

void Ball::updateInterpolate(NetworkBall& nball)
{
	glm::vec3 targetBallPos = glm::vec3((float)(nball.x / 1000.0f), 0.0f, (float)(nball.z / 1000.0f));
	float interpolationFactor = 0.05f; // Ajustable selon les besoins


	this->position = glm::mix(this->position, targetBallPos, interpolationFactor);

	updateModelMatrixFromPosition();

	this->velocityX = (float)(nball.velocityX / 1000.0f);
	this->velocityZ = (float)(nball.velocityZ / 1000.0f);
	this->moveSpeed = nball.speed;
}

void Ball::update(NetworkBall& nball)
{
    if(velocityX )

	this->position.x = (float)(nball.x / 1000.0f);
	this->position.z = (float)(nball.z / 1000.0f);

	updateModelMatrixFromPosition();

	this->velocityX = (float)(nball.velocityX / 1000.0f);
	this->velocityZ = (float)(nball.velocityZ / 1000.0f);
	this->moveSpeed = nball.speed;

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
