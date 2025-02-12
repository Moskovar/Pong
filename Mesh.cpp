#include "Mesh.h"
#include <GLM/gtc/type_ptr.hpp>


Mesh::Mesh(std::vector<Vertex> v_vertices, std::vector<unsigned int> v_indices)
{
	printf("Total vertices: %d\n", v_vertices.size());
	this->v_vertices = v_vertices;
	this->v_indices  = v_indices;
	VAO = 0;
	VBO = 0;
	IBO = 0;
	indexCount = 0;

	for (Vertex v : this->v_vertices)
	{
		//printf("OFFICIAL: VertexID: %d ... BoneID: %d ... Weight: %f\n", v.id, v.bonesID.x, v.weights.x);
	}

	//--- Création et bind des VAO VBO IBO ---//
	unsigned int numOfVertices = v_vertices.size();
	unsigned int* indices = &v_indices[0];
	unsigned int numOfIndices = v_indices.size();
	indexCount = numOfIndices;

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * v_vertices.size(), v_vertices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, v_indices.size() * sizeof(unsigned int), v_indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(0);

	// Attribut des coordonnées de texture
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bonesID));
	glEnableVertexAttribArray(5);

	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, weights));
	glEnableVertexAttribArray(6);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	//updateMesh();
}

void Mesh::updateMesh()
{
	

	
}

void Mesh::RenderMesh()
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::ClearMesh()
{
	if (IBO != 0)
	{
		glDeleteBuffers(1, &IBO);
		IBO = 0;
	}

	if (VBO != 0)
	{
		glDeleteBuffers(1, &VBO);
		VBO = 0;
	}

	if (VAO != 0)
	{
		glDeleteVertexArrays(1, &VAO);
		VAO = 0;
	}

	indexCount = 0;
}

Mesh::~Mesh()
{
	ClearMesh();
}
