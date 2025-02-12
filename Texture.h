#pragma once
#include <GL/glew.h>

#include <iostream>

class Texture
{
	public:
		Texture() {}
		Texture(std::string filePath);
		~Texture();

		void useTexture();
		void useTexture(GLenum textureUnit);

	private:
		GLuint textureID = 0;
		int width = 0, height = 0, nrChannels = 0;
		const char* filePath = "";
};

