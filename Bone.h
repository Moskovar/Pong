#pragma once
#include <GLM/glm.hpp>             // Pour les types de base (vec3, mat4, etc.)
#include <GLM/gtc/quaternion.hpp> //  Pour les quaternions (quat)
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include <assimp/scene.h>
#include <vector>
#include <map>
#include <string>
#include <tuple>

#include "Mesh.h"

struct KeyFrame 
{
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;
	//double time;
};

class Bone
{
	public:
		Bone() {};
		Bone(int id, std::string name, aiMatrix4x4 offsetMatrix);
		~Bone();

		auto getAnimations()				{ return keyFrames; }
		std::string getName()				{ return name; }
		glm::mat4 getOffsetMatrix()			{ return offsetMatrix; }
		void addChildren(Bone* bone)		{ children.push_back(bone); }
		std::vector<Bone*> getChildren()	{ return children; }

		void setKeyFrames(unsigned short animationID, std::map<double, KeyFrame> keyFrames) { this->keyFrames[animationID] = keyFrames; }
		void interpolateTransform(unsigned short animationID, double animationTime, glm::mat4* bonesTransform, glm::mat4& parentTransforms);
		
	private:
		int id = 0;
		std::string name;
		std::map<unsigned short, std::map<double, KeyFrame>> keyFrames;
		std::vector<Bone*> children;
		glm::mat4 offsetMatrix;

		// Fonction pour interpoler entre deux valeurs
		glm::vec3 interpolate(const glm::vec3& start, const glm::vec3& end, float factor) { return start + factor * (end - start); }
}; 

