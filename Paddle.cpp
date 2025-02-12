#include "Paddle.h"

Paddle::Paddle(short id, glm::vec3 position, const std::string& filePath) : Element(id, position, filePath)
{
	width = abs(getMaxPoint().z) + abs(getMinPoint().z);

	std::cout << "Width: " << width << std::endl;
}
