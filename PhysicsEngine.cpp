#include "PhysicsEngine.h"

PhysicsEngine::PhysicsEngine(std::vector<Player*>* players, std::vector<Element>* walls, Ball* ball)
{
    this->players   = players;
    this->walls     = walls;
    this->ball      = ball;
}

GLfloat PhysicsEngine::distanceBetweenHitboxes(Element* e1, Element* e2)
{
    GLfloat distanceX = 0.0f;
    GLfloat distanceY = 0.0f;
    GLfloat distanceZ = 0.0f;

    glm::vec3 e1_maxPoint = e1->getRHitbox().maxPoint;
    glm::vec3 e1_minPoint = e1->getRHitbox().minPoint;
    glm::vec3 e2_maxPoint = e2->getRHitbox().maxPoint;
    glm::vec3 e2_minPoint = e2->getRHitbox().minPoint;

    // Axe X
    if (e1_maxPoint.x < e2_minPoint.x)
    {
        distanceX = e2_minPoint.x - e1_maxPoint.x;  // À gauche
    }
    else if (e1_minPoint.x > e2_maxPoint.x)
    {
        distanceX = e1_minPoint.x - e2_maxPoint.x;  // À droite
    }

    // Axe Y
    if (e1_maxPoint.y < e2_minPoint.y)
    {
        distanceY = e2_minPoint.y - e1_maxPoint.y;  // En-dessous
    }
    else if (e1_minPoint.y > e2_maxPoint.y)
    {
        distanceY = e1_minPoint.y - e2_maxPoint.y;  // Au-dessus
    }

    // Axe Z
    if (e1_maxPoint.z < e2_minPoint.z)
    {
        distanceZ = e2_minPoint.z - e1_maxPoint.z;  // Derrière
    }
    else if (e1_minPoint.z > e2_maxPoint.z)
    {
        distanceZ = e1_minPoint.z - e2_maxPoint.z;  // Devant
    }

    // Si les hitboxes se chevauchent, retourner 0
    if (distanceX <= 0.0f && distanceY <= 0.0f && distanceZ <= 0.0f)
    {
        return 0.0f;  // Collision
    }

    // Calculer la distance totale entre les deux hitboxes en 3D
    return glm::sqrt(distanceX * distanceX + distanceY * distanceY + distanceZ * distanceZ);
}

void PhysicsEngine::run(GLfloat& deltaTime)
{
    //std::cout << ball->getPosition().z << std::endl;
    for (Player* p : *players)
    {
        GLfloat distance = distanceBetweenHitboxes(p->getPaddle(), ball);
        if (distance == 0)
        {
            //std::cout << "DISTANCE == 0" << std::endl;

            ball->turnBack();

            GLfloat velocityZ = (ball->getPosition().z - p->getPaddle()->getPosition().z) / (p->getPaddle()->getWidth() / 2);

            if (p->getSide() == 1) velocityZ *= -1;

            std::cout << ball->getPosition().z - p->getPaddle()->getPosition().z << std::endl;
            std::cout << velocityZ << std::endl;

            ball->setVelocityZ(velocityZ);

            while (distanceBetweenHitboxes(p->getPaddle(), ball) == 0)
                ball->move(deltaTime);
            
            return;
        }
        else if (p->getSide() == ball->getDirection())//si la balle se dirige dans la direction du joueur
        {
            if (p->getSide() == -1 && ball->getPosition().x < p->getPaddle()->getPosition().x - 5.0f || p->getSide() == 1 && ball->getPosition().x > p->getPaddle()->getPosition().x + 5.0f) 
                ball->setPositionX(15);
        }
    }

    for (Element& wall : *walls)
    {
        GLfloat distance = distanceBetweenHitboxes(&wall, ball);

        if (distance == 0)
        {
            ball->setVelocityZ(-ball->getVelocityZ());

            while (distanceBetweenHitboxes(&wall, ball) == 0) ball->move(deltaTime);
        }
    }
}
