#version 330 core

in vec4 vertexColor;
in vec2 TexCoords; // Coordonnées de texture en entrée du vertex shader

out vec4 FragColor;

uniform vec3 color; // Couleur uniforme

uniform sampler2D texture1; // Sampler pour la texture

void main()
{
    FragColor = texture(texture1, TexCoords); // Récupère la couleur de la texture en utilisant les coordonnées de texture
    //FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}