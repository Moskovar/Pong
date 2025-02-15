#include "Element.h"

Element::Element(short id, glm::vec3 position, const std::string& filePath)
{
	model = new Model(filePath);
	this->id = id;

	this->position = position;

	translate(position);

	updatePosition();

	startTime = std::chrono::high_resolution_clock::now();
}

Element::Element(short id, glm::mat4 modelMtx, const std::string& filePath)
{
	model = new Model(filePath);
	this->id = id;

	this->modelMatrix = glm::mat4(modelMtx);

	updatePosition();

	startTime = std::chrono::high_resolution_clock::now();
}

Element::Element(short id, glm::vec3 position, Model* model)
{
	this->id = id;
	this->position = position;
	this->model = model;

	//std::cout << "MODEL ADDR: " << model << std::endl;

	translate(position);

	updatePosition();

	//std::cout << "COPIE" << std::endl;
	startTime = std::chrono::high_resolution_clock::now();
}

Element::~Element()
{
	//if (model)
	//{
	//	delete model;
	//	model = nullptr;
	//}
	printf("||--- Element is cleared ---||\n");
}

void Element::setYaw(GLfloat yaw)
{
	this->yaw = yaw;
	modelMatrix = glm::rotate(modelMatrix, glm::radians(this->yaw - yaw), glm::vec3(0.0f, 1.0f, 0.0f));
	updatePosition();
}

void Element::setMove(bool state)
{
	if (!isFalling()) moving = state;
}

void Element::setFall(bool state)
{
	falling = state;
	if (!falling && movingValue == 0) moving = false;
}

void Element::setModelMtx(glm::mat4 modelMatrix)
{
	this->modelMatrix = modelMatrix;
	updatePosition();
}

/// <summary>
/// Récupère la position de l'élément si le mouvement qu'il s'apprête à faire s'effectue
/// </summary>
/// <param name="deltaTime"></param>
/// <returns> retourne la position anticipée</returns>
glm::mat4 Element::getAnticipatedMove(GLfloat deltaTime)
{
	glm::mat4 mtx = modelMatrix;
	mtx = glm::translate(mtx, glm::vec3(0.0f, 0.0f, moveSpeed * deltaTime));
	return mtx;
}

OBB Element::getAnticipatedMoveHitbox(GLfloat deltaTime)//duplication code avec fall
{
	glm::mat4 anticipatedMtx = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, moveSpeed * deltaTime));

	OBB anticipatedHitbox;
	anticipatedHitbox.center = glm::vec3(anticipatedMtx[3].x, anticipatedMtx[3].y, anticipatedMtx[3].z);

	// Assurer que l'orientation est correcte
	anticipatedHitbox.orientation = glm::mat3(anticipatedMtx);

	glm::vec3 scale = glm::vec3(
		glm::length(this->modelMatrix[0]),
		glm::length(this->modelMatrix[1]),
		glm::length(this->modelMatrix[2])
	);

	// Recalculer halfSize en tenant compte des transformations
	anticipatedHitbox.halfSize = (model->getMaxPoint() - model->getMinPoint()) * 0.5f * scale;

	anticipatedHitbox.updateBounds();

	return anticipatedHitbox;
}

OBB Element::getOBBAtPos(glm::vec3 pos)//duplication code avec fall
{
	glm::mat4 anticipatedMtx = modelMatrix;
	anticipatedMtx[3].x = pos.x;
	anticipatedMtx[3].y = pos.y;
	anticipatedMtx[3].z = pos.z;

	OBB anticipatedHitbox;
	anticipatedHitbox.center = glm::vec3(anticipatedMtx[3].x, anticipatedMtx[3].y, anticipatedMtx[3].z);

	// Assurer que l'orientation est correcte
	anticipatedHitbox.orientation = glm::mat3(anticipatedMtx);

	glm::vec3 scale = glm::vec3(
		glm::length(this->modelMatrix[0]),
		glm::length(this->modelMatrix[1]),
		glm::length(this->modelMatrix[2])
	);

	// Recalculer halfSize en tenant compte des transformations
	anticipatedHitbox.halfSize = (model->getMaxPoint() - model->getMinPoint()) * 0.5f * scale;

	anticipatedHitbox.updateBounds();

	return anticipatedHitbox;
}

OBB Element::getAnticipatedFallHitbox(GLfloat deltaTime)//duplication code avec getAntiMoveBoxe
{
	glm::mat4 anticipatedMtx = glm::translate(modelMatrix, glm::vec3(0.0f, -FALL_SPEED * deltaTime, 0.0f));

	OBB anticipatedHitbox;
	anticipatedHitbox.center = glm::vec3(anticipatedMtx[3].x, anticipatedMtx[3].y, anticipatedMtx[3].z);

	// Assurer que l'orientation est correcte
	anticipatedHitbox.orientation = glm::mat3(anticipatedMtx);

	glm::vec3 scale = glm::vec3(
		glm::length(this->modelMatrix[0]),
		glm::length(this->modelMatrix[1]),
		glm::length(this->modelMatrix[2])
	);

	// Recalculer halfSize en tenant compte des transformations
	anticipatedHitbox.halfSize = (model->getMaxPoint() - model->getMinPoint()) * 0.5f * scale;

	anticipatedHitbox.updateBounds();

	return anticipatedHitbox;
}

void Element::moveForward(GLfloat z)//deltatime??
{
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, z));
	updatePosition();
}

void Element::moveZ(GLfloat deltaTime)
{
	translate(glm::vec3(0.0f, 0.0f, moveSpeed * deltaTime));
	updatePosition();
}

void Element::moveAtY(GLfloat positionY)
{
	translate(glm::vec3(0.0f, positionY - position.y, 0.0f));
	updatePosition();
}

void Element::moveAtZ(GLfloat positionz)
{
	translate(glm::vec3(0.0f, 0.0f, position.z - position.z));
	updatePosition();
}

void Element::translate(glm::vec3 translation)
{
	//model->translate(modelMatrix, translation);
	modelMatrix = glm::translate(modelMatrix, translation);
	updatePosition();
}

void Element::turn(GLfloat yaw)//et le deltatime ??
{
	this->yaw += yaw;
	if (this->yaw > 360) this->yaw -= 360;
	else if (this->yaw < -360) this->yaw += 360;

	modelMatrix = glm::rotate(modelMatrix, glm::radians(-yaw), glm::vec3(0.0f, 1.0f, 0.0f));
	updatePosition();
	//std::cout << this->yaw << std::endl;
}

void Element::fall(GLfloat deltaTime)
{
	translate(glm::vec3(0.0f, -FALL_SPEED * deltaTime, 0.0f));
	updatePosition();
}

void Element::calculateHitBox()
{
	// Mettre à jour le centre de l'OBB
	hitbox.center = this->position;

	// Extraire l'orientation depuis la matrice de modèle
	hitbox.orientation = glm::mat3(this->modelMatrix);

	// Calculer l'échelle à partir des colonnes de la matrice de modèle
	glm::vec3 scale(
		glm::length(this->modelMatrix[0]),  // Échelle sur l'axe X
		glm::length(this->modelMatrix[1]),  // Échelle sur l'axe Y
		glm::length(this->modelMatrix[2])   // Échelle sur l'axe Z
	);

	// Calculer halfSize en prenant en compte l'échelle
	hitbox.halfSize = (model->getMaxPoint() - model->getMinPoint()) * 0.5f * scale;

	//std::cout << hitbox.orientation[0].x << " : " << hitbox.orientation[0].y << " : " << hitbox.orientation[0].z << std::endl;
	//std::cout << hitbox.orientation[1].x << " : " << hitbox.orientation[1].y << " : " << hitbox.orientation[1].z << std::endl;
	//std::cout << hitbox.orientation[2].x << " : " << hitbox.orientation[2].y << " : " << hitbox.orientation[2].z << std::endl;

	hitbox.updateBounds();
}


void Element::updatePosition()
{
	this->position.x = this->modelMatrix[3].x;
	this->position.y = this->modelMatrix[3].y;
	this->position.z = this->modelMatrix[3].z;

	calculateHitBox();
}

void Element::updateModelMatrixFromPosition()
{
	this->modelMatrix[3].x = this->position.x;
	this->modelMatrix[3].y = this->position.y;
	this->modelMatrix[3].z = this->position.z;

	calculateHitBox();
}



void Element::clear()
{
	if (model)
	{
		delete model;
		model = nullptr;
	}
}

void Element::render(const GLuint& modelLoc, const GLuint& bonesTransformsLoc)
{
	float ticksPerSecond = 0, duration = 0, animationTime = 0;
	glm::mat4 bonesTransform[NUM_BONES] = {};
	if (model->getAnimations().size() > 0)
	{
		//printf("AnimationID: %d\n", animationID);
		//printf("AnimationID: %d ... Duration: %f\n", animationID, model->getAnimation(animationID)->getDuration());
		ticksPerSecond = model->getAnimation(animationID)->getTicksPerSecond();
		duration = model->getAnimation(animationID)->getDuration() / ticksPerSecond;

		auto currentTime = std::chrono::high_resolution_clock::now();
		float timeSinceStart = std::chrono::duration<float>(currentTime - startTime).count();

		animationTime = fmod(timeSinceStart * ticksPerSecond, duration);

		glm::mat4 rootMatrix = glm::mat4(1.0f);
		model->getAnimation(animationID)->getRootBone()->interpolateTransform(animationID, animationTime, bonesTransform, rootMatrix);

		glUniformMatrix4fv(bonesTransformsLoc, NUM_BONES, GL_FALSE, &bonesTransform[0][0][0]);
	}

	//glUniform1i(idLoc, id);//debug - affichage couleurs différentes

	//Passer les matrices de vue et de projection aux shaders
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	model->renderModel();
}

