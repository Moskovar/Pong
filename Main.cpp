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

std::mutex mtx;

Window*         window          = nullptr;
Camera*         camera          = nullptr;
PhysicsEngine*  physicsEngine   = nullptr;
Ball*           ball            = nullptr;

glm::mat4*  view    = nullptr;
glm::mat4   projection;

std::map<char, bool>    keyPressed;
std::map<int, bool>     mousePressed;

std::vector<Player*> players;

GLboolean run = true;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        keyPressed[key] = true;

        switch (key)
        {
        case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, true);   break;
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
    }
}

glm::vec3 generateRayFromCursor(GLFWwindow* window)
{
    // 1. R�cup�rer la position du curseur
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    //std::cout << xpos << " : " << ypos << std::endl;

    // 2. Convertir en coordonn�es NDC
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    float xNDC = (2.0f * xpos) / width - 1.0f;
    float yNDC = 1.0f - (2.0f * ypos) / height;  // Inverser Y

    // 3. Passer � l'espace vue
    glm::vec4 clipCoords = glm::vec4(xNDC, yNDC, -1.0f, 1.0f);
    glm::vec4 eyeCoords = glm::inverse(projection) * clipCoords;
    eyeCoords = glm::vec4(eyeCoords.x, eyeCoords.y, -1.0f, 0.0f);

    // 4. Passer � l'espace monde
    glm::vec4 worldCoords = glm::inverse(*view) * eyeCoords;
    glm::vec3 rayDirection = glm::normalize(glm::vec3(worldCoords));

    // Maintenant rayDirection est la direction dans le monde vers laquelle le curseur pointe.
    //std::cout << "RayDirection: " << rayDirection.x << " : " << rayDirection.y << " : " << rayDirection.z << std::endl;
    return rayDirection;
}

bool findRayIntersectionWithMap(const glm::vec3& rayOrigin, const glm::vec3& rayDirection, glm::vec3& worldPos) {
    // V�rifie que le rayon n'est pas parall�le au plan
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

    while (run)
    {
        headerReceived = co.recvUDP(np, nb);
        switch(headerReceived)
        {
            case Header::NP:   players[1]->update(np);  break;
            case Header::BALL: ball->update(nb);        break;
        }

        //std::cout << "Coucou thread" << std::endl;
        //std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void receive_data_tcp()
{
    NetworkBall nball;

    while (run)
    {
        if (co.recvTCP(nball, run))
        {
            mtx.lock();
            if (ball) ball->update(nball);
            else std::cout << "BALL nullptr" << std::endl;
            mtx.unlock();
        }
    }
}

int main()
{
	std::cout << "Hello Pong !" << std::endl;

    //co.setWaitingModeTCP(true);

    bool runt = true;
    NetworkPaddleStart nps1, nps2;
    co.recvNPSTCP(nps1, runt);
    //std::cout << "HEADER: " << nps1.header << " : " << nps1.id << " : " << nps1.side << std::endl;
    co.recvNPSTCP(nps2, runt);
    //std::cout << "HEADER: " << nps2.header << " : " << nps2.id << " : " << nps2.side << std::endl;

    //co.setWaitingModeTCP(false);

    //--- Chargement du contexte OpenGL ---//
    window = new Window(width, height);
    window->fullScreen();
    window->keepCursorInWindow();
    window->createCallbacks();
    GLFWwindow* glfwWindow = window->getGLFWWindow();

    glfwSetKeyCallback(glfwWindow, keyCallback);
    glfwSetMouseButtonCallback(glfwWindow, mouse_button_callback);  // Clics de souris

    glm::vec3 target = glm::vec3(0.0, 0.0f, 0.0f);

    //--- Chargement de la cam�ra ---//
    camera = new Camera();

    view = camera->getViewMatrixP();
    projection = glm::perspective(glm::radians(45.0f), width / height, 0.1f, 100.0f);

    //--- Chargement des shaders ---//
    std::map<std::string, Shader> shaders;
    shaders["AnimatedObject"] = Shader("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl", view, &projection);

    // Activer le test de profondeur
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    auto    startTime       = std::chrono::high_resolution_clock::now();
    float   currentFrame    = 0, animationTime = 0, timeSinceStart = 0,
            lastFrame       = glfwGetTime(), deltaTime = 0, now = 0;

    players.push_back(new Player(nps1.gameID, nps1.id, nps1.side));
    players.push_back(new Player(nps2.gameID, nps2.id, nps2.side));

    ball = new Ball(0, glm::vec3(0.0f, 0.0f, 0.0f)  , "models/fbx/ball.fbx");

    std::vector<Element> walls;
    walls.push_back(Element(0, glm::vec3(0.0f, 0.0f, -40.0f), "models/fbx/wall.fbx"));
    walls.push_back(Element(0, glm::vec3(0.0f, 0.0f,  40.0f), "models/fbx/wall.fbx"));

    physicsEngine = new PhysicsEngine(&players, &walls, ball);

    std::thread t_receive_data_udp(receive_data_udp);
    //std::thread t_receive_data_tcp(receive_data_tcp);

    std::cout << "MAXPOINT: " << ball->getRHitbox().maxPoint.x << " : " << ball->getRHitbox().maxPoint.y << " : " << ball->getRHitbox().maxPoint.z << std::endl;

    std::cout << "MINPOINT: " << walls[0].getRHitbox().minPoint.x << " : " << walls[0].getRHitbox().minPoint.y << " : " << walls[0].getRHitbox().minPoint.z << std::endl;
    std::cout << "MAXPOINT: " << walls[0].getRHitbox().maxPoint.x << " : " << walls[0].getRHitbox().maxPoint.y << " : " << walls[0].getRHitbox().maxPoint.z << std::endl;
    std::cout << "MINPOINT: " << walls[1].getRHitbox().minPoint.x << " : " << walls[1].getRHitbox().minPoint.y << " : " << walls[1].getRHitbox().minPoint.z << std::endl;
    std::cout << "MAXPOINT: " << walls[1].getRHitbox().maxPoint.x << " : " << walls[1].getRHitbox().maxPoint.y << " : " << walls[1].getRHitbox().maxPoint.z << std::endl;
    //std::cout << "MAXPOINT: " << ball->getRHitbox().maxPoint.x << " : " << ball->getRHitbox().maxPoint.y << " : " << ball->getRHitbox().maxPoint.z << std::endl;

    while (!glfwWindowShouldClose(glfwWindow) && run)
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        timeSinceStart = std::chrono::duration<float>(currentTime - startTime).count();

        //DeltaTime
        now = glfwGetTime();
        deltaTime = now - lastFrame;
        lastFrame = now;

        glm::vec3 worldPos;
        findRayIntersectionWithMap(camera->getPosition(), generateRayFromCursor(glfwWindow), worldPos);

        std::cout << "worldPos: " << worldPos.x << " : " << worldPos.y << " : " << worldPos.z << std::endl;

        //if (players[0]->getNP().id == 0) std::cout << worldPos.z << std::endl;

        if (players[0])
        {
            GLfloat z = players[0]->getPaddle()->getPosition().z;
            players[0]->getPaddle()->setPositionZ(worldPos.z);
            if (players[0]->getPaddle()->getPosition().z != z)
            {
                //std::cout << "pos changed, send udp..." << std::endl;
                NetworkPaddle np = players[0]->getNP();
                //std::cout << players[0]->getPaddle()->getPosition().z << " : " << np.z << std::endl;
                co.sendNPUDP(np);
            }
        }

        // Effacer le buffer de couleur et de profondeur
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Draw
        shaders["AnimatedObject"].use();
        for (Player* p : players)
            p->getPaddle()->render(shaders["AnimatedObject"].modelLoc, shaders["AnimatedObject"].bonesTransformsLoc);

        ball->render(shaders["AnimatedObject"].modelLoc, shaders["AnimatedObject"].bonesTransformsLoc);
        for(Element& wall : walls) wall.render(shaders["AnimatedObject"].modelLoc, shaders["AnimatedObject"].bonesTransformsLoc);

        //physicsEngine->run(deltaTime);
        ball->move(deltaTime);
        
        //--- Reset des mouvements souris dans la fen�tre pour traiter les prochains ---//
        window->resetXYChange();

        //Swap buffers
        glfwSwapBuffers(glfwWindow);
        glfwPollEvents();
    }

    run = false;

    t_receive_data_udp.join();
    //t_receive_data_tcp.join();

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

    if (window)
    {
        delete window;
        window = nullptr;
    }

    return 0;
}