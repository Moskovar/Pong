#version 330 core

#define NUM_BONES 100

layout(location = 0) in vec3  pos; // Position des sommets
layout(location = 1) in vec2  texCoords; // Coordonnées de texture
layout(location = 5) in ivec4 boneIDs;
layout(location = 6) in vec4  boneWeights;

out vec4 vertexColor;
out vec2 TexCoords; // Passer les coordonnées de texture au fragment shader

uniform mat4 model; // Matrice de modele
uniform mat4 view;  // Matrice de vue
uniform mat4 projection; // Matrice de projection
uniform mat4 bonesTransform[NUM_BONES];

void main()
{
    vec4 totalPosition = vec4(0.0);

    for (int i = 0; i < 4; i++)
    {
        if (boneIDs[i] != -1) // Ignorer les IDs d'os non valides
        {
            mat4 boneTransform = bonesTransform[boneIDs[i]];
            totalPosition += boneTransform * vec4(pos, 1.0) * boneWeights[i];
        }
    }

    if (boneIDs[0] != -1) 
    {
        gl_Position = projection * view * model * totalPosition;
    }
    else
    {
        gl_Position = projection * view * model * vec4(pos, 1.0);
    }

    TexCoords = texCoords; // Assigner les coordonnées de texture à la variable de sortie
}