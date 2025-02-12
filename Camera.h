#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>

#include <iostream>
#include <map>

using namespace std;

class Camera
{
	public:
		Camera();
		~Camera();

		//--- Getters ---//
		GLfloat		getYaw()				{ return yaw;							}
		glm::mat4*	getViewMatrixP()		{ return &viewMatrix;					}
		GLfloat		getSensitivity()		{ return sensitivity;					}
		glm::vec3	getUp()					{ return glm::vec3(0.0f, 1.0f, 0.0f);	}
		glm::vec3	getPosition()			{ return position;						}

		void addYaw(GLfloat yaw);
		void addPitch(GLfloat pitch);
		void setRadius(GLfloat radius);

		void mouseControl(GLFWwindow* window, GLfloat xChange, GLfloat yChange, GLfloat& scrollValue, const float& deltaTime);
		void update();
		void updateViewMatrix();

	private:
		glm::mat4  viewMatrix;
		glm::vec3  position, front, right;
		glm::vec3* target;
		GLfloat yaw = 0.0f, pitch = -90.0f, radius = 100.0f, sensitivity = 150.0f;
		map<char, bool>* keyPressed = nullptr;


};

