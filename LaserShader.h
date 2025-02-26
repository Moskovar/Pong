#pragma once
#include "Shader.h"
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

class LaserShader : public Shader
{
	public:
		LaserShader(std::string vertexSrc, std::string fragmentSrc, glm::mat4* view, glm::mat4* projection, float* timeValue, glm::vec3* laserCenter);

		void use() override;

		GLuint timeLoc = 0, laserCenterLoc = 0;
		float* timeValue = nullptr;
		glm::vec3* laserCenter = nullptr;

	private:

};

