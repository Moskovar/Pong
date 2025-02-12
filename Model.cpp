#include "Model.h"
#include "uti.hpp"

Model::Model(const std::string& filePath)
{
	loadModel(filePath);
	//printf("Model has %d nodes\n", nodes.size());
	for (Node* node : nodes)
	{
		printf("Node: %s has %d children\n", node->name.c_str(), node->children.size());
		if (bones[node->name])
		{
			for (Node* childNode : node->children)
			{
				printf("     -> ChildName: %s\n", childNode->name.c_str());
				if (bones[childNode->name])
					bones[node->name]->addChildren(bones[childNode->name]);
			}
		}
	}

	for (Node* node : nodes)
	{
		if (node)
		{
			delete node;
			node = nullptr;
		}
	}

	for (auto& animation : animations)
	{
		printf("Animation name: %s\n", animation.second.getName().c_str());
	}

	//std::cout << "MIN_POINT: " << hitBox.min_point.x << " ... " << hitBox.min_point.y << " ... " << hitBox.min_point.z << std::endl;
	//std::cout << "MAX_POINT: " << hitBox.max_point.x << " ... " << hitBox.max_point.y << " ... " << hitBox.max_point.z << std::endl;
}

void Model::loadModel(const std::string& fileName)
{
	//clearModel();

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(fileName, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices);

	if (!scene || !scene->mRootNode)
	{
		printf("Model (%s) failed to load: %s", fileName, importer.GetErrorString());
		return;
	}

	loadNode(scene->mRootNode, scene);

	printf("\n\n\n");
	printf("Nombre d'animation pour: %s -> %d\n", fileName.c_str(), scene->mNumAnimations);
	// Vérifiez si le modèle contient des animations
	for(int animationNumber = 0; animationNumber < scene->mNumAnimations; ++animationNumber)
	{
		const aiAnimation* animation = scene->mAnimations[animationNumber]; // Récupère la première animation
		double ticksPerSecond = animation->mTicksPerSecond / ANIMATION_SPEED_RATE;
		// Vous pouvez maintenant travailler avec l'animation
		// Variables pour gérer le temps
		printf("Animation name: %s\n", animation->mName.C_Str());
		printf("Duree d'animation: %f\nNombre de tick par seconde: %f\n", animation->mDuration, animation->mTicksPerSecond);

		std::string animationName = animation->mName.C_Str();
		int animationID = animationIDs[animationName];

		std::cout << "ANIMATION-NAME: " << animationName << std::endl;

		for (unsigned int i = 0; i < animation->mNumChannels; i++)//on parcourt os de l'animation
		{
			const aiNodeAnim* bone = animation->mChannels[i];
			std::map<double, KeyFrame> keyFrames;

			//if (bones[std::string(bone->mNodeName.C_Str())]) printf("bone name: %s ... Vertices: %d\n", bone->mNodeName.C_Str(), bones[std::string(bone->mNodeName.C_Str())]->getVertices().size());
			//else printf("Bone %s is not set\n", bone->mNodeName.C_Str());
			for (unsigned int j = 0; j < bone->mNumPositionKeys; j++) 
			{
				keyFrames[bone->mPositionKeys[j].mTime / ticksPerSecond].position.x = bone->mPositionKeys[j].mValue.x;
				keyFrames[bone->mPositionKeys[j].mTime / ticksPerSecond].position.y = bone->mPositionKeys[j].mValue.y;
				keyFrames[bone->mPositionKeys[j].mTime / ticksPerSecond].position.z = bone->mPositionKeys[j].mValue.z;
				//printf("Position:  x: %f ... y: %f ... z: %f  -> Time: %f\n", keyFrames[bone->mPositionKeys[j].mTime / ticksPerSecond].position.x, keyFrames[bone->mPositionKeys[j].mTime / ticksPerSecond].position.y, keyFrames[bone->mPositionKeys[j].mTime / ticksPerSecond].position.z, bone->mPositionKeys[j].mTime / ticksPerSecond);
				//printf("Time: pos : %f\n", bone->mPositionKeys[j].mTime);
			}

			//printf("\n");

			for (unsigned int j = 0; j < bone->mNumRotationKeys; j++)
			{
				keyFrames[bone->mRotationKeys[j].mTime / ticksPerSecond].rotation.w = bone->mRotationKeys[j].mValue.w;
				keyFrames[bone->mRotationKeys[j].mTime / ticksPerSecond].rotation.x = bone->mRotationKeys[j].mValue.x;
				keyFrames[bone->mRotationKeys[j].mTime / ticksPerSecond].rotation.y = bone->mRotationKeys[j].mValue.y;
				keyFrames[bone->mRotationKeys[j].mTime / ticksPerSecond].rotation.z = bone->mRotationKeys[j].mValue.z;
				//printf("Rotation:  w: %f ... x: %f ... y: %f ... z: %f  -> Time: %f\n", keyFrames[bone->mRotationKeys[j].mTime / ticksPerSecond].rotation.w, keyFrames[bone->mRotationKeys[j].mTime / ticksPerSecond].rotation.x, keyFrames[bone->mRotationKeys[j].mTime / ticksPerSecond].rotation.y, keyFrames[bone->mRotationKeys[j].mTime / ticksPerSecond].rotation.z, bone->mRotationKeys[j].mTime / ticksPerSecond);
				//printf(" ... rot : %f\n", bone->mRotationKeys[j].mTime);
			}

			//printf("\n");
			
			for (unsigned int j = 0; j < bone->mNumScalingKeys; j++)
			{
				
				keyFrames[bone->mScalingKeys[j].mTime / ticksPerSecond].scale.x = bone->mScalingKeys[j].mValue.x;
				keyFrames[bone->mScalingKeys[j].mTime / ticksPerSecond].scale.y = bone->mScalingKeys[j].mValue.y;
				keyFrames[bone->mScalingKeys[j].mTime / ticksPerSecond].scale.z = bone->mScalingKeys[j].mValue.z;
				//printf("Scale: x: %f ... y: %f ... z: %f  -> Time: %f\n", bone->mScalingKeys[j].mValue.x, bone->mScalingKeys[j].mValue.y, bone->mScalingKeys[j].mValue.z, bone->mScalingKeys[j].mTime / ticksPerSecond);
				//printf(" ... scal : %f\n" , bone->mScalingKeys[j].mTime);
			}

			
			if (bones[std::string(bone->mNodeName.C_Str())]) bones[std::string(bone->mNodeName.C_Str())]->setKeyFrames(animationID, keyFrames);
			//else printf("%s is not set \n", bone->mNodeName.C_Str());
		}
		animations[animationID] = Animation(std::string(animation->mName.C_Str()), animation->mDuration, ticksPerSecond, bones["Bone"]);
	}

	//for (auto it = nodes.begin(); it != nodes.end(); it++) printf("%s\n", it->first.c_str());//if(it->second) printf("Node name : % s\n", it->second->getName().c_str());

	//printf("\nNombre de noeuds total: %d", nodes.size());

	loadMaterials(scene);

	//if(!meshList.empty())
		for (auto& vertex : meshList[0]->getVertices())
		{
			float weightSum = vertex.weights.x + vertex.weights.y + vertex.weights.z + vertex.weights.w;
			if (weightSum > 0.0f) 
			{
				//printf("Vertex ID: %d ... weightSum: %f\n", vertex.id, weightSum);
				vertex.weights /= weightSum;
				weightSum = vertex.weights.x + vertex.weights.y + vertex.weights.z + vertex.weights.w;
				printf("Vertex ID: %d ... weightSum: %f\n", vertex.id, weightSum);
			}

		

			if ((vertex.bonesID.x == 3 || vertex.bonesID.y == 3 || vertex.bonesID.z == 3 || vertex.bonesID.w == 3)
			 && (vertex.bonesID.x == 4 || vertex.bonesID.y == 4 || vertex.bonesID.z == 4 || vertex.bonesID.w == 4))
			{
				std::cout << "Vertex ID: " << vertex.id << " is common" << std::endl;
			}
		}
}

void Model::renderModel()
{
	for (size_t i = 0; i < meshList.size(); i++)
	{
		unsigned int materialIndex = meshToTex[i];

		//if (materialIndex < textureList.size() && textureList[materialIndex])
		//{
		//	textureList[materialIndex]->UseTexture();
		//}

		if (textures.size() > 0 && textures[0]) textures[0]->useTexture();
		else std::cout << "PAS DE TEXTURE !!" << std::endl;

		meshList[i]->updateMesh();
		meshList[i]->RenderMesh();
	}
}

void Model::clearModel()
{
	
}

Model::~Model()
{
	for (size_t i = 0; i < meshList.size(); i++)
	{
		if (meshList[i])
		{
			delete meshList[i];
			meshList[i] = nullptr;
		}
	}

	for (auto it = bones.begin(); it != bones.end(); ++it)
	{
		if (it->second)
		{
			delete it->second;
			it->second = nullptr;
		}
	}

	//for (size_t i = 0; i < textureList.size(); i++)
	//{
	//	if (textureList[i])
	//	{
	//		delete textureList[i];
	//		textureList[i] = nullptr;
	//	}
	//}

	printf("||--- Model is cleared ---||\n");
}

void Model::loadNode(aiNode* node, const aiScene* scene, Node* parentNode)
{
	//if(node->mNumChildren > 0) printf("Nom du noeud: %s ... nombre de meshes: %d\n", node->mName.C_Str(), node->mNumMeshes);

	std::vector<Mesh*> meshes;

	Node* currentNode = new Node{ std::string(node->mName.C_Str()) };
	nodes.push_back(currentNode);
	//printf("Node: %s\n", currentNode->name.c_str());

	if (parentNode) parentNode->children.push_back(currentNode);//Si le node actuel a un parent, on lui ajout le node actuel comme enfant

	for (size_t i = 0; i < node->mNumMeshes; i++) 
	{
		loadMesh(scene->mMeshes[node->mMeshes[i]], scene, meshes);
	}

	for (size_t i = 0; i < node->mNumChildren; i++)
	{ 
		loadNode(node->mChildren[i], scene, currentNode);
	}

	//bones[std::string(node->mName.C_Str())] = Bone(std::string(node->mName.C_Str()), meshes);
}
//#include <glm/gtx/string_cast.hpp>
void Model::loadMesh(aiMesh* mesh, const aiScene* scene, std::vector<Mesh*>& meshes)
{
	std::vector<Vertex>	  v_vertices;
	std::vector<unsigned int> v_indices;

	for (size_t i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		vertex.id = i;

		vertex.position.x = mesh->mVertices[i].x;
		vertex.position.y = mesh->mVertices[i].y;
		vertex.position.z = mesh->mVertices[i].z;

		if (vertex.position.x < minPoint.x) minPoint.x = vertex.position.x;
		if (vertex.position.y < minPoint.y) minPoint.y = vertex.position.y;
		if (vertex.position.z < minPoint.z) minPoint.z = vertex.position.z;

		if (vertex.position.x > maxPoint.x) maxPoint.x = vertex.position.x;
		if (vertex.position.y > maxPoint.y) maxPoint.y = vertex.position.y;
		if (vertex.position.z > maxPoint.z) maxPoint.z = vertex.position.z;

		// Récupérer les normales
		if (mesh->HasNormals())
		{
			vertex.normal.x = mesh->mNormals[i].x;
			vertex.normal.y = mesh->mNormals[i].y;
			vertex.normal.z = mesh->mNormals[i].z;
		}

		// Récupérer les coordonnées de texture
		if (mesh->mTextureCoords[0])
		{
			vertex.texCoords.x = mesh->mTextureCoords[0][i].x;
			vertex.texCoords.y = mesh->mTextureCoords[0][i].y;
		}
		else vertex.texCoords = glm::vec2(0.0f, 0.0f);

		v_vertices.push_back(vertex);
	}

	if (mesh->HasBones()) 
	{//useless car si 0 on rentre pas dans tableau ?!
		for (size_t i = 0; i < mesh->mNumBones; i++)
		{
			aiBone* bone = mesh->mBones[i];
			printf("Bone: %s has %d vertices\n", bone->mName.C_Str(), bone->mNumWeights);
			std::vector<std::tuple<float, Vertex>> v_vertices2;//enlever l'autre vecteur pointeur?
			for (size_t j = 0; j < bone->mNumWeights; j++)
			{
				for (Vertex& vertex : v_vertices)//on cherche le vertex correspondant dans le vecteur qu'on vient de remplir et quand on trouve on le met dans le vecteur pour l'os
					if (vertex.id == bone->mWeights[j].mVertexId)
					{
						//vertex->weight = bone->mWeights[j].mWeight;
						if		(vertex.bonesID.x == -1) { vertex.bonesID.x = i; vertex.weights.x = bone->mWeights[i].mWeight; }
						else if (vertex.bonesID.y == -1) { vertex.bonesID.y = i; vertex.weights.y = bone->mWeights[i].mWeight; }
						else if (vertex.bonesID.z == -1) { vertex.bonesID.z = i; vertex.weights.z = bone->mWeights[i].mWeight; }
						else if (vertex.bonesID.w == -1) { vertex.bonesID.w = i; vertex.weights.w = bone->mWeights[i].mWeight; }
						//if (std::string(bone->mName.C_Str()) == "Head") printf("Head ID: %d\n", i);
						//printf("Bone x: %f ... y: %f ... z: %f !!!!\n", bone->mOffsetMatrix[3]);
						//printf("ID: %d, poids: %f\n", vertex.bonesID.x, vertex.weights.x);
						//v_vertices2.push_back(std::make_tuple(bone->mWeights[j].mWeight, vertex));
					}
			}
			bones[std::string(bone->mName.C_Str())] = new Bone(i, std::string(bone->mName.C_Str()), bone->mOffsetMatrix);
			//printf("Bone: %s has %d vertices\n", bone->mName.C_Str(), bones[std::string(bone->mName.C_Str())].getVertices().size());
		}
	}

	std::cout << "MINPOINT: " << minPoint.x << " ... " << minPoint.y << " ... " << minPoint.z << std::endl;
	std::cout << "MAXPOINT: " << maxPoint.x << " ... " << maxPoint.y << " ... " << maxPoint.z << std::endl;

	for (size_t i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (size_t j = 0; j < face.mNumIndices; j++)
		{
			v_indices.push_back(face.mIndices[j]);
		}
	}

	Mesh* newMesh = new Mesh(v_vertices, v_indices);
	meshList.push_back(newMesh);
	meshes.push_back(newMesh);
	meshToTex.push_back(mesh->mMaterialIndex);
}

void Model::loadMaterials(const aiScene* scene)
{
	for (size_t i = 0; i < scene->mNumMaterials; i++)
	{
		aiMaterial* material = scene->mMaterials[i];

		if (material->GetTextureCount(aiTextureType_DIFFUSE))
		{
			aiString path;
			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)
			{
				int idx = std::string(path.data).rfind("\\");
				std::string filename = std::string(path.data).substr(idx + 1);

				std::string texPath = std::string("textures/") + filename;

				std::cout << "TexPath: " << texPath << std::endl;

				textures.push_back(new Texture(texPath.c_str()));
			}
		}
	}
}
