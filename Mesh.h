#pragma once
#include <GL/glew.h>
#include <GLM/gtc/matrix_transform.hpp>
#include <vector>

struct Vertex
{
	unsigned int id = 0;
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
	glm::ivec4 bonesID;
	glm::vec4 weights;

	Vertex() : bonesID(-1, -1, -1, -1) {};

	Vertex(unsigned int id, glm::vec3 position, glm::vec3 normal, glm::vec2 texCoords, glm::ivec4 bonesID, glm::vec4 weights) 
	{
		this->id = id;
		this->position = position;
		this->normal = normal;
		this->texCoords = texCoords;
		this->bonesID = bonesID;
		this->weights = weights;
	}

	bool operator==(const Vertex& other) const
	{
		return id == other.id;
	}
};

class Mesh
{
	public:
		Mesh(std::vector<Vertex> v_vertices, std::vector<unsigned int> indices);

		auto getVertices() { return v_vertices; }

		void updateMesh();
		void RenderMesh();
		void ClearMesh();

		~Mesh();

	private:
		GLuint VAO, VBO, IBO;
		GLsizei indexCount;
		std::vector<Vertex>  v_vertices;//contient tous les vertices de la mesh -> attribué dans createMesh
		std::vector<unsigned int> v_indices;//à voir à quoi ça correspond
};

