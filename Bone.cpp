#include "Bone.h"
#include <iostream>

Bone::Bone(int id, std::string name, aiMatrix4x4 offsetMatrix)
{
    this->id = id;
    this->name = name;
    this->offsetMatrix = glm::mat4
    {
        offsetMatrix.a1, offsetMatrix.b1, offsetMatrix.c1, offsetMatrix.d1,
        offsetMatrix.a2, offsetMatrix.b2, offsetMatrix.c2, offsetMatrix.d2,
        offsetMatrix.a3, offsetMatrix.b3, offsetMatrix.c3, offsetMatrix.d3,
        offsetMatrix.a4, offsetMatrix.b4, offsetMatrix.c4, offsetMatrix.d4
    };
}

Bone::~Bone()
{

}

void Bone::interpolateTransform(unsigned short animationID, double animationTime, glm::mat4* bonesTransform, glm::mat4& parentTransforms)
{
    glm::mat4 res = parentTransforms;
    //const std::map<double, KeyFrame> keyFrames = keyFrames[animation];
    //printf("KeyFrame SIZE: %d\n", keyFrames.size());


    //std::cout << animationID << std::endl;
    //std::cout << &keyFrames << std::endl;
    uintptr_t specificAddress = 0x30;

    // Vérifier si le pointeur est nul ou si l'adresse est spécifique
    if (&keyFrames == nullptr || reinterpret_cast<uintptr_t>(&keyFrames) == specificAddress)//pourquoi ça fait ça ??
    {
        //std::cout << "Pointeur est nul ou egale a l'adresse specifique : 0x30" << std::endl;
        return;
    }

    // Si aucune keyframe n'est disponible, retourner une matrice identité
    if (keyFrames[animationID].empty())
    {
        bonesTransform[id] = res * offsetMatrix;
        for (Bone* child : children)
            child->interpolateTransform(animationID, animationTime, bonesTransform, res);
        return;
    }

    // Trouver les keyframes qui entourent le temps d'animation donné
    auto nextKeyFrame = keyFrames[animationID].lower_bound(animationTime);

    if (nextKeyFrame == keyFrames[animationID].end()) // Si aucune keyframe suivante n'est trouvée, utiliser la dernière keyframe
    {
        std::cout << "LASLAST" << std::endl;
        res *= glm::translate(keyFrames[animationID].rbegin()->second.position) * glm::mat4_cast(keyFrames[animationID].rbegin()->second.rotation) * glm::scale(keyFrames[animationID].rbegin()->second.scale);
        bonesTransform[id] = res * offsetMatrix;
        for(Bone* child : children)
            child->interpolateTransform(animationID, animationTime, bonesTransform, res);
        return;
    }

    if (nextKeyFrame == keyFrames[animationID].begin()) // Si aucune keyframe précédente n'est trouvée, utiliser la première keyframe
    {
        res *= glm::translate(keyFrames[animationID].begin()->second.position) * glm::mat4_cast(keyFrames[animationID].begin()->second.rotation) * glm::scale(keyFrames[animationID].begin()->second.scale);
        for (Bone* child : children)
            child->interpolateTransform(animationID, animationTime, bonesTransform, res);
        return;
    }

    auto prevKeyFrame = std::prev(nextKeyFrame);

    //printf("AnimTime: %f ... KeyFramesSize: %d ... prev: %f ... next: %f\n", animationTime, keyFrames.size(), prevKeyFrame->first, nextKeyFrame->first);

    // Calculer le facteur d'interpolation
    float factor = static_cast<float>((animationTime - prevKeyFrame->first) / (nextKeyFrame->first - prevKeyFrame->first));
    // Interpoler les positions, rotations et échelles
    glm::vec3 position = glm::mix(prevKeyFrame->second.position, nextKeyFrame->second.position, factor);
    glm::quat rotation = glm::slerp(prevKeyFrame->second.rotation, nextKeyFrame->second.rotation, factor);
    glm::vec3 scale    = interpolate(prevKeyFrame->second.scale, nextKeyFrame->second.scale, factor);

    res *= glm::translate(glm::mat4(1.0f), position) * glm::mat4_cast(rotation) * glm::scale(glm::mat4(1.0f), scale);
    bonesTransform[id] = res * offsetMatrix;
    for (Bone* child : children)
        child->interpolateTransform(animationID, animationTime, bonesTransform, res);

    return;
}
