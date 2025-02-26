#include "uti.hpp"

std::map <int, std::string> gl_textures_string =
{
	{ GL_TEXTURE0, "texture0" },
	{ GL_TEXTURE1, "texture1" },
	{ GL_TEXTURE2, "texture2" },
	{ GL_TEXTURE3, "texture3" }
};

bool isPointInOBB(const glm::vec3& point, const OBB& obb) {
	// Transformer le point dans le système de coordonnées de l'OBB
	glm::vec3 pointInLocal = glm::inverse(obb.orientation) * (point - obb.center);

	// Vérifier si le point est dans les limites de l'OBB
	return (pointInLocal.x >= -obb.halfSize.x && pointInLocal.x <= obb.halfSize.x) &&
		(pointInLocal.y >= -obb.halfSize.y && pointInLocal.y <= obb.halfSize.y) &&
		(pointInLocal.z >= -obb.halfSize.z && pointInLocal.z <= obb.halfSize.z);
}

std::map<std::string, int> animationIDs =
{
	{ "Armature|Run"	, 1 },
	{ "Armature|Attack"	, 2}
};

uint64_t getCurrentTimestamp()
{
	return static_cast<uint64_t>(std::time(nullptr));
}

long long getCurrentTimestampMs()
{
	auto now = std::chrono::system_clock::now();
	auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
	return now_ms.time_since_epoch().count();
}
