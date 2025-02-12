#pragma once
#include <string>
#include <GL/glew.h>
#include <GLM/glm.hpp>

class Shader
{
	public:
		Shader() {};
		Shader(std::string vertexSrc, std::string fragmentSrc, glm::mat4* view, glm::mat4* projection);

		GLuint getShaderProgram() { return shaderProgram; }
		//GLuint getUniformLocation(const char* uniformName) { return glGetUniformLocation(shaderProgram, uniformName); }

		void use();

		GLuint colorLoc = 0, viewLoc = 0, projLoc = 0, modelLoc = 0, bonesTransformsLoc = 0;

	private:
		GLuint shaderProgram = 0;

		glm::mat4* view = nullptr, *projection = nullptr;

		std::string readShaderSource(std::string filePath);
		GLuint compileShader(const char* source, GLenum shaderType);
		GLuint createShaderProgram(std::string vertexPath, std::string fragmentPath);
};

