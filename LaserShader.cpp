#include "LaserShader.h"

LaserShader::LaserShader(std::string vertexSrc, std::string fragmentSrc, glm::mat4* view, glm::mat4* projection, float* timeValue) : Shader(vertexSrc, fragmentSrc, view, projection)
{
	this->timeValue = timeValue;

	timeLoc = glGetUniformLocation(shaderProgram, "time");

	std::cout << "Laser shader created" << std::endl;
}

void LaserShader::use()
{
	Shader::use();
	//std::cout << *timeValue << std::endl;
	//if(timeValue)
	glUniform1f(timeLoc, *timeValue);
}
