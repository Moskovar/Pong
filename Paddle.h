#pragma once
#include "Element.h"

class Paddle : public Element
{
	public:
		Paddle() {}
		Paddle(short id, glm::vec3 position, const std::string& filePath);

		GLfloat getWidth() { return width; }

	private:
		GLfloat width = 0.0f;
};

