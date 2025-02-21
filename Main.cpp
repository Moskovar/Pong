#include <iostream>
#include <chrono>
#include <map>

#include <thread>
#include <mutex>

#include "Window.h"
#include "Camera.h"
#include "Shader.h"

#include "Connection.h"
#include "PhysicsEngine.h"

#include "Player.h"
#include "Ball.h"

GLfloat width = 1920.0f, height = 1080.0f;

Connection co;

std::mutex mtx, mtx_ball;

Window*         window          = nullptr;
Camera*         camera          = nullptr;
PhysicsEngine*  physicsEngine   = nullptr;
Ball*           ball            = nullptr;

glm::mat4*  view    = nullptr;
glm::mat4   projection;

std::map<std::string, Shader>   shaders;
std::map<char, bool>            keyPressed;
std::map<int, bool>             mousePressed;
std::map<Element*, bool>        menu_buttons_hovered;

std::vector<Player*> players;
std::vector<Element> walls;

//--- Menu ---//
Element* play = nullptr, * leave_mm = nullptr;

GLboolean b_run = true, b_run_menu = true, b_run_game = false, b_run_matchmaking = false;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        keyPressed[key] = true;

        switch (key)
        {
            case GLFW_KEY_ESCAPE:   
                glfwSetWindowShouldClose(window, true);
                if (b_run_menu) b_run = false;
                break;
            case GLFW_KEY_Q:        /*co.sendNSTCP({Header::SPELL, SpellID::GRAVITY});*/ break;
        }
    }   
    else if (action == GLFW_RELEASE)
    {
        keyPressed[key] = false;
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (action == GLFW_PRESS)      mousePressed[button] = true;
    else if (action == GLFW_RELEASE)
    {
        mousePressed[button] = false;

        if (menu_buttons_hovered[play])
        {
            co.sendMatchmakingTCP();

            b_run_matchmaking = !b_run_matchmaking;
            std::cout << b_run_matchmaking << std::endl;
        }
    }
}

glm::vec3 generateRayFromCursor(GLFWwindow* window)
{
    // 1. Récupérer la position du curseur
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    //std::cout << xpos << " : " << ypos << std::endl;

    // 2. Convertir en coordonnées NDC
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    float xNDC = (2.0f * xpos) / width - 1.0f;
    float yNDC = 1.0f - (2.0f * ypos) / height;  // Inverser Y

    // 3. Passer à l'espace vue
    glm::vec4 clipCoords = glm::vec4(xNDC, yNDC, -1.0f, 1.0f);
    glm::vec4 eyeCoords = glm::inverse(projection) * clipCoords;
    eyeCoords = glm::vec4(eyeCoords.x, eyeCoords.y, -1.0f, 0.0f);

    // 4. Passer à l'espace monde
    glm::vec4 worldCoords = glm::inverse(*view) * eyeCoords;
    glm::vec3 rayDirection = glm::normalize(glm::vec3(worldCoords));

    // Maintenant rayDirection est la direction dans le monde vers laquelle le curseur pointe.
    //std::cout << "RayDirection: " << rayDirection.x << " : " << rayDirection.y << " : " << rayDirection.z << std::endl;
    return rayDirection;
}

float distanceBetweenHitboxes(Element* e1, Element* e2)
{
    float distanceX = 0.0f;
    float distanceY = 0.0f;
    float distanceZ = 0.0f;

    if (!e1 || !e2) std::cout << "E1 OR E2 NULLPTR !!!!" << std::endl;

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

float distanceBetweenHitboxes(OBB& obb1, OBB& obb2)
{
    float distanceX = 0.0f;
    float distanceY = 0.0f;
    float distanceZ = 0.0f;

    //if (!e1 || !e2) std::cout << "E1 OR E2 NULLPTR !!!!" << std::endl;

    glm::vec3 e1_maxPoint = obb1.maxPoint;
    glm::vec3 e1_minPoint = obb1.minPoint;
    glm::vec3 e2_maxPoint = obb2.maxPoint;
    glm::vec3 e2_minPoint = obb2.minPoint;

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

bool findRayIntersectionWithMap(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, glm::vec3& worldPos) {
    // Vérifie que le rayon n'est pas parallèle au plan
    const float epsilon = 0.0001f;
    if (fabs(rayDirection.y) < epsilon || rayDirection.y >= 0.0f) {
        return false;
    }

    // Calcul de t
    float t = -rayOrigin.y / rayDirection.y;

    // Calcul du point d'intersection
    worldPos = rayOrigin + t * rayDirection;
    return true;
}

void receive_data_udp()
{
    NetworkPaddle np;
    NetworkBall   nb;

    short headerReceived = -1;

    while (b_run_game)
    {
        headerReceived = co.recvUDP(np, nb);
        switch(headerReceived)
        {
            case Header::NP:   if(players[1]) players[1]->update(np);  break;
            //case Header::BALL: ball->update(nb);        break;
            case Header::BALL: 
                //std::cout << "Ball          -> " << ball->getX() << " : " << ball->getZ() << std::endl;
                mtx_ball.lock();
                ball->updateInterpolate(nb);
                mtx_ball.unlock();
                //std::cout << "Ball received -> " << (float)(nb.x / 1000.0f) << " : " << (float)(nb.z / 1000.0f) << std::endl;
                break;
                
        }

        //std::cout << "Coucou thread" << std::endl;
        //std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void receive_data_tcp()
{
    NetworkBall nball;
    short header = 0;

    while (b_run_game)
    {
        header = co.recvTCP(nball, b_run_game);
        if(header == Header::BALL)
        {
            //mtx.lock();
            if (ball)
            {
                mtx_ball.lock();
                ball->update(nball);
                mtx_ball.unlock();
            }
            else std::cout << "BALL nullptr" << std::endl;
            //mtx.unlock();
        }
    }
}

void run_menu()
{    
    menu_buttons_hovered[play]      = false;
    menu_buttons_hovered[leave_mm]  = false;

    auto    startTime = std::chrono::high_resolution_clock::now();
    float   currentFrame = 0, animationTime = 0, timeSinceStart = 0,
            lastFrame = glfwGetTime(), deltaTime = 0, now = 0;

    GLFWwindow* glfwWindow = window->getGLFWWindow();

    while (!glfwWindowShouldClose(glfwWindow) && b_run_menu)
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        timeSinceStart = std::chrono::duration<float>(currentTime - startTime).count();

        //DeltaTime
        now = glfwGetTime();
        deltaTime = now - lastFrame;
        lastFrame = now;

        glm::vec3 worldPos;
        findRayIntersectionWithMap(camera->getPosition(), generateRayFromCursor(glfwWindow), worldPos);

        //std::cout << "WorldPos: " << worldPos.z << std::endl;

        //--- Gestion du bouton play ---//
        bool inPlay = false;
        Element* button = nullptr;
        if (!b_run_matchmaking)
        {
            inPlay = isPointInOBB(worldPos, play->getRHitbox());
            button = play;
        }
        else
        {
            inPlay = isPointInOBB(worldPos, leave_mm->getRHitbox());
            button = leave_mm;
        }

        if (inPlay && button->getRotations().x == 0.0f)
        {
            button->turn(-25, glm::vec3(1.0f, 0.0f, 0.0f), false);
            menu_buttons_hovered[button] = true;
        }
        else if (!inPlay && button->getRotations().x != 0.0f)
        {
            button->resetRotations();
            menu_buttons_hovered[button] = false;
        }
        ///--- Fin gestion bouton play ---//
        

        // Effacer le buffer de couleur et de profondeur
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shaders["AnimatedObject"].use();
        if(!b_run_matchmaking)  play->render(shaders["AnimatedObject"].modelLoc, shaders["AnimatedObject"].bonesTransformsLoc);
        else                    leave_mm->render(shaders["AnimatedObject"].modelLoc, shaders["AnimatedObject"].bonesTransformsLoc);

        //--- Reset des mouvements souris dans la fenêtre pour traiter les prochains ---//
        window->resetXYChange();

        //Swap buffers
        glfwSwapBuffers(glfwWindow);
        glfwPollEvents();
    }

    b_run_menu = false;//mettre à false pour si on press escape -> glfwWindowShouldClose
}

void run_game()
{
    //--- Chargement du jeu ---// //créer un chargement après que la game soit créée
    std::cout << "Waiting for matchmaking..." << std::endl;

    bool runt = true;
    NetworkPaddleStart nps1, nps2;
    co.recvNPSTCP(nps1, runt);
    co.recvNPSTCP(nps2, runt);

    std::thread t_receive_data_udp(receive_data_udp);
    std::thread t_receive_data_tcp(receive_data_tcp);

    walls.push_back(Element(0, glm::vec3(0.0f, 0.0f, -40.0f), "models/fbx/wall.fbx"));
    walls.push_back(Element(0, glm::vec3(0.0f, 0.0f, 40.0f), "models/fbx/wall.fbx"));

    //std::cout << "MINPOINT: " << walls[0].getRHitbox().minPoint.x << " : " << walls[0].getRHitbox().minPoint.y << " : " << walls[0].getRHitbox().minPoint.z << std::endl;
    //std::cout << "MAXPOINT: " << walls[0].getRHitbox().maxPoint.x << " : " << walls[0].getRHitbox().maxPoint.y << " : " << walls[0].getRHitbox().maxPoint.z << std::endl;
    //std::cout << "MINPOINT: " << walls[1].getRHitbox().minPoint.x << " : " << walls[1].getRHitbox().minPoint.y << " : " << walls[1].getRHitbox().minPoint.z << std::endl;
    //std::cout << "MAXPOINT: " << walls[1].getRHitbox().maxPoint.x << " : " << walls[1].getRHitbox().maxPoint.y << " : " << walls[1].getRHitbox().maxPoint.z << std::endl;
    //std::cout << "MAXPOINT: " << ball->getRHitbox().maxPoint.x << " : " << ball->getRHitbox().maxPoint.y << " : " << ball->getRHitbox().maxPoint.z << std::endl;

    players.push_back(new Player(nps1.gameID, nps1.id, nps1.side));
    players.push_back(new Player(nps2.gameID, nps2.id, nps2.side));

    camera->setSide(players[0]->getSide());

    ball = new Ball(0, glm::vec3(0.0f, 0.0f, 0.0f), "models/fbx/ball.fbx");

    physicsEngine = new PhysicsEngine(&players, &walls, ball, &mtx_ball);
    //--- Fin chargement jeu ---//

    auto    startTime = std::chrono::high_resolution_clock::now();
    float   currentFrame = 0, animationTime = 0, timeSinceStart = 0,
            lastFrame = glfwGetTime(), deltaTime = 0, now = 0;

    GLFWwindow* glfwWindow = window->getGLFWWindow();

    while (!glfwWindowShouldClose(glfwWindow) && b_run_game)
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        timeSinceStart = std::chrono::duration<float>(currentTime - startTime).count();

        //DeltaTime
        now = glfwGetTime();
        deltaTime = now - lastFrame;
        lastFrame = now;

        glm::vec3 worldPos;
        findRayIntersectionWithMap(camera->getPosition(), generateRayFromCursor(glfwWindow), worldPos);

        //std::cout << "WorldPos: " << worldPos.z << std::endl;

        if (players[0])
        {
            glm::vec3 wpos = players[0]->getPaddle()->getPosition();
            wpos.z = worldPos.z;

            OBB obb2 = players[0]->getPaddle()->getOBBAtPos(wpos);

            for (Element& wall : walls)
            {
                if (distanceBetweenHitboxes(obb2, wall.getRHitbox()) == 0)
                {
                    OBB wallOBB = wall.getRHitbox();
                    worldPos.z = (worldPos.z > 0) ? wallOBB.center.z - wallOBB.halfSize.z - players[0]->getPaddle()->getWidth() / 2.0f //pour les obstacles > 0
                        : wallOBB.center.z + wallOBB.halfSize.z + players[0]->getPaddle()->getWidth() / 2.0f;//pour les obstacles < 0 
                    break;
                }
            }

            GLfloat z = players[0]->getPaddle()->getPosition().z;
            players[0]->getPaddle()->setPositionZ(worldPos.z);
            if (players[0]->getPaddle()->getPosition().z != z)//si le paddle a changé de position
            {
                //std::cout << "pos changed, send udp..." << std::endl;
                NetworkPaddle np = players[0]->getNP();
                //std::cout << players[0]->getPaddle()->getPosition().z << " : " << np.z << std::endl;
                co.sendNPUDP(np);
            }
        }

        glm::vec3 maxp = walls[0].getMaxPoint(), minp = walls[0].getMinPoint();
        glm::vec3 maxp2 = walls[0].getModel()->getMaxPoint(), minp2 = walls[0].getModel()->getMinPoint();

        //std::cout << maxp.x << " : " << maxp.y << " : " << maxp.z << std::endl;
        //std::cout << minp.x << " : " << minp.y << " : " << minp.z << std::endl;
        //std::cout << maxp2.x << " : " << maxp2.y << " : " << maxp2.z << std::endl;
        //std::cout << minp2.x << " : " << minp2.y << " : " << minp2.z << std::endl;

        // Effacer le buffer de couleur et de profondeur
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Draw
        shaders["AnimatedObject"].use();
        for (Player* p : players)
            p->getPaddle()->render(shaders["AnimatedObject"].modelLoc, shaders["AnimatedObject"].bonesTransformsLoc);

        ball->render(shaders["AnimatedObject"].modelLoc, shaders["AnimatedObject"].bonesTransformsLoc);
        for (Element& wall : walls) wall.render(shaders["AnimatedObject"].modelLoc, shaders["AnimatedObject"].bonesTransformsLoc);

        //play.render(shaders["AnimatedObject"].modelLoc, shaders["AnimatedObject"].bonesTransformsLoc);

        physicsEngine->run(deltaTime);
        //ball->move(deltaTime);

        //--- Reset des mouvements souris dans la fenêtre pour traiter les prochains ---//
        window->resetXYChange();

        //Swap buffers
        glfwSwapBuffers(glfwWindow);
        glfwPollEvents();
    }

    b_run       = false;
    b_run_game  = false;

    t_receive_data_udp.join();
    t_receive_data_tcp.join();
}

int main()
{
	std::cout << "Hello Pong !" << std::endl;

    NetworkVersion nv;
    co.recvVersionTCP(nv);

    if (nv.version != 1010) return 0;

    //--- Chargement du contexte OpenGL ---//
    window = new Window(width, height);
    window->fullScreen();
    window->keepCursorInWindow();
    window->createCallbacks();
    GLFWwindow* glfwWindow = window->getGLFWWindow();

    glfwSetKeyCallback(glfwWindow, keyCallback);
    glfwSetMouseButtonCallback(glfwWindow, mouse_button_callback);  // Clics de souris

    glm::vec3 target = glm::vec3(0.0, 0.0f, 0.0f);

    //--- Chargement de la caméra ---//
    camera = new Camera();

    view = camera->getViewMatrixP();
    projection = glm::perspective(glm::radians(45.0f), width / height, 0.1f, 120.0f);

    //--- Chargement des shaders ---//
    shaders["AnimatedObject"] = Shader("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl", view, &projection);

    play        = new Element(0, glm::vec3(0.0f, 0.0f, -25.0f), "models/fbx/menu/fr/play.fbx");
    leave_mm    = new Element(0, glm::vec3(0.0f, 0.0f, -25.0f), "models/fbx/menu/fr/leave_mm.fbx");

    // Activer le test de profondeur
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    while (b_run)
    {
        if (b_run_menu)
        {
            run_menu();
        }
        else if (b_run_game)
        {
            run_game();
        }
    }

    // Nettoyer et quitter
    if (physicsEngine)
    {
        delete physicsEngine;
        physicsEngine = nullptr;
    }

    for (Player* p : players)
    {
        if (p)
        {
            delete p;
            p = nullptr;
        }
    }

    if (ball)
    {
        delete ball;
        ball = nullptr;
    }

    if (play)//charger au lancement et décharger à la fermeture
    {
        delete play;
        play = nullptr;
    }

    if (leave_mm)
    {
        delete leave_mm;
        leave_mm = nullptr;
    }

    if (window)
    {
        delete window;
        window = nullptr;
    }

    return 0;
}