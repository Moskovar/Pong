#pragma once
#include <GL/glew.h>
#include <GLM/glm.hpp>
#include <map>
#include <string>
#include <array>
#include <iostream>
#include <mutex>

#define CAM_MAX_DISTANCE 125

//--- Shaders ---//
#define MAP_JUNCTIONS_SHADERS				"map_shaders"
#define CHUNKS_JUNCTIONS_SHADERS			"chunks_shaders"
#define LARGETILES_SHADERS					"largetiles_shaders"

#define MAX_HEIGHT		500
#define TURN_SPEED		300		//character turn speed
#define FALL_SPEED		25.0f	//character fall speed
#define JUMP_HIGH		4.0f
#define WORLD_Y_OFFSET	100.0f

#define TILE_SIZE 32

#define LARGETILE_SIZE				512
#define LARGETILE_ARR_SIZE			16

#define CHUNK_ARR_SIZE 2 //2x2

#define MAP_ARR_SIZE 2 //2x2 donc Map = 2x2x2x2 LargesTiles -> MAP_ARR_SIZE² * CHUNK_ARR_SIZE² 

//nombre de side pour les jointures dans LT (par paires)		 //* 2 car chaque jointure prendre les vertices haut d'une tuile et bas de la suivante (donc pas réutilisable d'une jointure à l'autre)
#define LT_JUNCTION_VERTICES_SIZE	2 * (LARGETILE_ARR_SIZE - 1)//Il y a TILE_SIZE - 1 joint à faire sur l'axe z (y du tableau) | y: nombre de Tile, x: nombre de vertex sur une rangée X d'une Tile	
																
//nombre de side pour les jointures dans CK (par paires)
#define CK_JUNCTION_VERTICES_SIZE	2 * (CHUNK_ARR_SIZE - 1)

//nombre de side pour les jointures dans MP (par paires)
#define MP_JUNCTION_VERTICES_SIZE	2 * (MAP_ARR_SIZE - 1)

struct AABB
{
	glm::vec3 min_point = glm::vec3(0.0f, 0.0f, 0.0f), max_point = glm::vec3(0.0f, 0.0f, 0.0f);
};

struct OBB 
{
	glm::vec3 center;  // Position centrale de la hitbox
	glm::mat3 orientation;  // Matrice 3x3 définissant les axes locaux (orientations)
	glm::vec3 halfSize;  // Moitié de la taille sur chaque axe
	glm::vec3 minPoint;
	glm::vec3 maxPoint;

	void updateBounds() 
	{
		glm::vec3 localMin = -halfSize;
		glm::vec3 localMax = halfSize;

		// Calculer les coins transformés (appliquer l'orientation sur chaque axe)
		glm::vec3 xAxis = orientation[0] * halfSize.x;
		glm::vec3 yAxis = orientation[1] * halfSize.y;
		glm::vec3 zAxis = orientation[2] * halfSize.z;

		// Calculer les 8 coins de l'OBB dans l'espace monde
		std::array<glm::vec3, 8> corners = 
		{
			center - xAxis - yAxis - zAxis,  // Coin inférieur arrière gauche
			center + xAxis - yAxis - zAxis,  // Coin inférieur arrière droit
			center - xAxis + yAxis - zAxis,  // Coin supérieur arrière gauche
			center + xAxis + yAxis - zAxis,  // Coin supérieur arrière droit
			center - xAxis - yAxis + zAxis,  // Coin inférieur avant gauche
			center + xAxis - yAxis + zAxis,  // Coin inférieur avant droit
			center - xAxis + yAxis + zAxis,  // Coin supérieur avant gauche
			center + xAxis + yAxis + zAxis   // Coin supérieur avant droit
		};

		// Initialiser minPoint et maxPoint à la première valeur
		minPoint = corners[0];
		maxPoint = corners[0];

		// Trouver les min et max sur chaque axe
		for (const auto& corner : corners) 
		{
			minPoint = glm::min(minPoint, corner);  // Trouve les composantes minimales
			maxPoint = glm::max(maxPoint, corner);  // Trouve les composantes maximales
		}

		//std::cout << "MP: " << maxPoint.x << " : " << maxPoint.y << " : " << maxPoint.z << std::endl;
		//std::cout << "mp: " << minPoint.x << " : " << minPoint.y << " : " << minPoint.z << std::endl;
	}
};

bool isPointInOBB(const glm::vec3& point, const OBB& obb);

struct HeightMapVertex
{
	GLfloat x = 0.0f, y = 0.0f, z = 0.0f;
	unsigned int indice = 0;
	glm::vec2 texCoords;
};

extern std::map <int, std::string>	gl_textures_string;

extern std::map<std::string, int> animationIDs;



//--- Network ---//
enum Header 
{
	VERSION,
	MATCHMAKING,
	NPS,
	NP,
	BALL,
	BALLSPEED,
	SPELL
};

enum SpellID
{
	GRAVITY
};

#pragma pack(push, 1)
struct NetworkVersion
{
	short header = Header::VERSION;
	int version = 0;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct NetworkMatchmaking
{
	short header = Header::MATCHMAKING;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct NetworkPaddleStart 
{
	short header = Header::NPS;
	short gameID = 0, id = 0, side = 0;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct NetworkPaddle 
{
	short header = Header::NP;
	short gameID = 0, id = -1;
	int z = 0, timestamp = -1;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct NetworkBall
{
	short header = Header::BALL;
	int x = 0, z = 0, velocityX = 0, velocityZ = 0;//short suffisant ?
	short speed = 0;
	uint32_t timestamp = 0;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct NetworkSpell
{
	short header = Header::SPELL;
	short spellID = 0;
};
#pragma pack(pop)

uint64_t getCurrentTimestamp();

uint64_t getCurrentTimestampMilliseconds();