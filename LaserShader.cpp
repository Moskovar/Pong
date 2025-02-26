#include "LaserShader.h"

LaserShader::LaserShader(std::string vertexSrc, std::string fragmentSrc, glm::mat4* view, glm::mat4* projection, float* timeValue, glm::vec3* laserCenter) : Shader(vertexSrc, fragmentSrc, view, projection)
{
	this->timeValue = timeValue;
	this->laserCenter = laserCenter;

	timeLoc			= glGetUniformLocation(shaderProgram, "time");
	laserCenterLoc	= glGetUniformLocation(shaderProgram, "center");

	std::cout << "LaserCenterLoc: " << laserCenterLoc << std::endl;
	std::cout << "TimeLoc: " << timeLoc << std::endl;
	std::cout << "ModelLoc: " << modelLoc << std::endl;

	std::cout << "LaserCenter: " << laserCenter->x << " : " << laserCenter->y << " : " << laserCenter->z << std::endl;
	//std::cout << "Laser shader created" << std::endl;
}

void LaserShader::use()
{
	Shader::use();
	//std::cout << *timeValue << std::endl;
	if(timeValue)
		glUniform1f(timeLoc, *timeValue);

	if (laserCenter)
	{
		//std::cout << "LaserCenterLoc: " << laserCenterLoc << std::endl;
		//std::cout << "TimeLoc: " << timeLoc << std::endl;
		//std::cout << "ModelLoc: " << modelLoc << std::endl;
		glUniform3fv(laserCenterLoc, 1, glm::value_ptr(*laserCenter));
	}
	//else std::cout << "LaserCenter nullptr" << std::endl;
}
