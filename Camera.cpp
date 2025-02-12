#include "Camera.h"
#include "uti.hpp"

Camera::Camera()
{
	// Position fixe de la caméra
	this->position = glm::vec3(0.0f, radius, 0.0f);  // Caméra au-dessus de la scène (axe Y)

	// La caméra regarde toujours vers le bas, donc pas besoin de calculer front et right
	this->viewMatrix = glm::lookAt(this->position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
}

Camera::~Camera()
{
}

void Camera::update()
{

    position.x = target->x + radius * cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    position.y = target->y + radius * sin(glm::radians(pitch));
    position.z = target->z + radius * cos(glm::radians(pitch)) * sin(glm::radians(yaw));

	
	this->front = glm::normalize(this->position - *target);
	this->right = glm::normalize(glm::cross(getUp(), front));
    viewMatrix = glm::lookAt(position, *target, getUp());
}

void Camera::updateViewMatrix()
{
	this->front = glm::normalize(this->position - *target);
	this->right = glm::normalize(glm::cross(getUp(), front));
	viewMatrix = glm::lookAt(position, *target, getUp());
}

void Camera::addYaw(GLfloat yaw)
{
	//std::cout << "addYaw" << std::endl;
	this->yaw += yaw;

	//std::cout << this->yaw << std::endl;

	if		(this->yaw >  360) this->yaw -= 360;
	else if (this->yaw < -360) this->yaw += 360;
}

void Camera::addPitch(GLfloat pitch)
{
	this->pitch += pitch;

	if		(this->pitch > 89) this->pitch = 89;
	else if (this->pitch < 1 ) this->pitch = 1;
}

void Camera::setRadius(GLfloat radius)
{
	this->radius -= radius;

	if      (this->radius > CAM_MAX_DISTANCE) this->radius = CAM_MAX_DISTANCE;
	else if (this->radius < 0 ) this->radius = 0;
}

void Camera::mouseControl(GLFWwindow* window, GLfloat mouseX, GLfloat mouseY, GLfloat& scrollValue, const float& deltaTime)
{
	//xChange *= turnSpeed;//USE TURNSPEED OU VELOCITY ???

	//if		(mouseX <= 5)		addYaw(deltaTime * sensitivity); 
	//else if (mouseX >= 1915)	addYaw(-deltaTime * sensitivity);

	//if (mouseY <= 5)		addPitch(deltaTime * sensitivity / 2);
	//else if (mouseY >= 1075)	addPitch(-deltaTime * sensitivity / 2);

	if		(mouseX <= 5    && this->target->z < 1088)	this->target->z +=  deltaTime * sensitivity;
	else if (mouseX >= 1915 && this->target->z > 964 )	this->target->z += -deltaTime * sensitivity;


	if (scrollValue != 0)
	{
		setRadius(scrollValue);
		scrollValue = 0;
	}

	update();
}
