#include "PhysicsEngine.h"

PhysicsEngine::PhysicsEngine(std::vector<Player*>* players, std::vector<Element>* walls, Ball* ball, std::mutex* mtx_ball)
{
    this->players   = players;
    this->walls     = walls;
    this->ball      = ball;
    this->mtx_ball  = mtx_ball;
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
    float distance;
    Element* element = nullptr;
    short isPaddle = true;//pour vérifier le type d'Element, plus rapide qu'un dynamic cast ?

    Player* p1 = ((*players)[0]->getSide() == -1) ? (*players)[0] : (*players)[1], * p2 = ((*players)[1]->getSide() == 1) ? (*players)[1] : (*players)[0];

    mtx_ball->lock();
    if (ball->getVelocityX() < 0 && p1)
    {
        element = p1->getPaddle();

        if (!element)
        {
            mtx_ball->unlock();
            return;
        }

        //std::cout << "P1" << std::endl;

        if (ball->getX() < element->getX() - 10)
        {
            std::cout << "OUT! P1" << std::endl;
            //return;
            //resetRound();
            //lastWinner = p2;
        }
    }
    else if (ball->getVelocityX() > 0 && p2)
    {
        element = p2->getPaddle();

        if (!element)
        {
            mtx_ball->unlock();
            return;
        }

        //std::cout << "P2" << std::endl;

        if (ball->getX() > element->getX() + 10)
        {
            std::cout << "OUT! P2" << std::endl;
            //return;
            //resetRound();
            //lastWinner = p1;
        }
    }
    //else return;

    if (!element)
    {
        mtx_ball->unlock();
        return;
    }

    distance = distanceBetweenHitboxes(ball, element);

    //std::cout << distance << std::endl;

    //Si la distance avec le joueur est > 0, alors on vérifie la distance avec les murs
    if (distance > 0)
    {
        for (Element& wall : *walls)
        {
            distance = distanceBetweenHitboxes(&wall, ball);

            if (distance == 0)//Si distance avec le mur == 0, on sort
            {
                element = &wall;
                isPaddle = false;//passe à un si on est au contact d'un wall, sinon reste à 0
                break;
            }
        }
    }

    //Si la distance avec le dernier élément comparé est > 0, on déplace la balle, sinon on traite la collision en fonction de l'élément
    if (distance > 0 || ball->getLastElementHit() == element)
    {
        ball->move(deltaTime);
    }
    else
    {
        if (isPaddle)
        {
            std::cout << "COLLISION AVEC PADDLE" << std::endl;

            //---- VelocityX ---//
            ball->turnBack();

            //--- VelocityZ ---//
            float velocityZ = (ball->getZ() - element->getZ()) / (static_cast<Paddle*>(element)->getWidth() / 2);


            ball->setVelocityZ(velocityZ);
        }
        else
        {
            ball->setVelocityZ(-ball->getVelocityZ());
        }

        ball->setLastElementHit(element);

        //while (distanceBetweenHitboxes(ball, element) == 0)
        //{
        //    ball->move(deltaTime);
        //}
    }
    mtx_ball->unlock();
}
