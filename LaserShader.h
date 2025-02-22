#pragma once
#include "Shader.h"
#include <iostream>
class LaserShader : public Shader
{
	public:
		LaserShader(std::string vertexSrc, std::string fragmentSrc, glm::mat4* view, glm::mat4* projection, float* timeValue);

		void use() override;

		GLuint timeLoc = 0;
		float* timeValue = nullptr;

	private:

};

