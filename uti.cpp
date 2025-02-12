#include "uti.hpp"

std::map <int, std::string> gl_textures_string =
{
	{ GL_TEXTURE0, "texture0" },
	{ GL_TEXTURE1, "texture1" },
	{ GL_TEXTURE2, "texture2" },
	{ GL_TEXTURE3, "texture3" }
};

std::map<int, AABB> obj_hitboxes =
{
	{}
};

int getCellCenter(GLfloat xOrZ, int cellWidth)
{
	return (int)xOrZ / 8 * 8 + 4;
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

uint64_t getCurrentTimestampMilliseconds()
{
	auto now = std::chrono::steady_clock::now();  // Utilise le clock steady pour plus de précision
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());  // Convertit en millisecondes
	return duration.count();  // Retourne le timestamp en millisecondes
}
