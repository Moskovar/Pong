#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture(std::string filePath)
{
	this->filePath = filePath.c_str();
    // Crée une variable pour stocker l'ID de la texture

    // Génère une texture et stocke son ID dans la variable texture
    glGenTextures(1, &textureID);

    // Lie la texture à la cible GL_TEXTURE_2D
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Utilise stb_image pour charger l'image de la texture depuis le chemin spécifié
    unsigned char* data = stbi_load(this->filePath, &width, &height, &nrChannels, 0);

    // Vérifie si l'image a été chargée avec succès
    if (data)
    {
        //printf("||--- Texture loaded ---||\n");
        // Déclare une variable pour stocker le format de l'image
        GLenum format;

        // Détermine le format de l'image en fonction du nombre de canaux
        if (nrChannels == 1)
            format = GL_RED;        // L'image a 1 canal (par exemple, une image en niveaux de gris)
        else if (nrChannels == 3)
            format = GL_RGB;        // L'image a 3 canaux (rouge, vert, bleu)
        else if (nrChannels == 4)
            format = GL_RGBA;       // L'image a 4 canaux (rouge, vert, bleu, alpha)

        //std::cout << "FORMAT: " << format << " : " << GL_RED << " : " << GL_RGB << " : " << GL_RGBA << std::endl;

        // Crée une texture en 2D avec les données de l'image
        // Les paramètres sont :
        // - Niveau de détail de la texture (0 = base level)
        // - Format interne de la texture
        // - Largeur et hauteur de l'image
        // - Toujours 0 (bordure, doit être 0)
        // - Format des pixels de l'image
        // - Type des données des pixels
        // - Pointeur vers les données de l'image
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

        // Génère des mipmaps pour la texture
        glGenerateMipmap(GL_TEXTURE_2D);

        // Définit les paramètres de wrapping (enveloppement) pour la coordonnée S (horizontale)
        // GL_TEXTURE_WRAP_S indique comment la texture est échantillonnée lorsqu'une coordonnée de texture dépasse la plage [0.0, 1.0]
        // GL_REPEAT répète la texture en cas de dépassement (comportement par défaut)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

        // Définit les paramètres de wrapping (enveloppement) pour la coordonnée T (verticale)
        // GL_TEXTURE_WRAP_T indique comment la texture est échantillonnée lorsqu'une coordonnée de texture dépasse la plage [0.0, 1.0]
        // GL_REPEAT répète la texture en cas de dépassement (comportement par défaut)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // Définit les paramètres de filtrage pour le minification (réduction) de la texture
        // GL_TEXTURE_MIN_FILTER indique quel type de filtrage est utilisé lorsque la texture est réduite
        // GL_LINEAR_MIPMAP_LINEAR utilise le filtrage linéaire avec les mipmaps pour une transition plus lisse entre les niveaux de détail
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        // Définit les paramètres de filtrage pour la magnification (agrandissement) de la texture
        // GL_TEXTURE_MAG_FILTER indique quel type de filtrage est utilisé lorsque la texture est agrandie
        // GL_LINEAR utilise le filtrage linéaire, ce qui signifie que la valeur de chaque pixel est interpolée linéairement entre les pixels voisins
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        // Affiche un message d'erreur si l'image n'a pas pu être chargée
        std::cout << "Failed to load texture" << std::endl;
    }

    // Libère la mémoire utilisée par les données de l'image
    stbi_image_free(data);
}

Texture::~Texture()
{
    filePath = (char*)"";
}

void Texture::useTexture()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
}

void Texture::useTexture(GLenum textureUnit)
{
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D, textureID);
}
