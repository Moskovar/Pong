#include "SpellBox.h"

SpellBox::SpellBox(short id, glm::vec3 position, const std::string& filePath) : Element(id, position, filePath)
{
}

void SpellBox::rotate(GLfloat angle, float& deltaTime)
{
	modelMatrix = glm::rotate(modelMatrix, angle * deltaTime, glm::vec3(1.0, 1.0, 1.0));
}
