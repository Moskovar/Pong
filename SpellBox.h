#pragma once
#include "Element.h"
class SpellBox : public Element
{
	public:
		SpellBox(short id, glm::vec3 position, const std::string& filePath);

		void rotate(GLfloat degree, float& deltaTime);

	private:
};

