#include "CGModel.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include "CGCamera.h"
#include "CGScene.h"
#include <iostream>
#include <cmath>

//
// FUNCIÓN: CGModel::initialize(int w, int h)
//
// PROPÓSITO: Inicializa el modelo 3D, shaders, cámara y escena
//
void CGModel::initialize(int w, int h)
{
    // Crea el programa gráfico para la escena
    sceneProgram = new CGShaderProgram("shaders/VertexShader.glsl", "shaders/FragmentShader.glsl");
    if (!sceneProgram->IsLinked()) return;

    // Crea el programa gráfico para el skybox
    skyboxProgram = new CGShaderProgram("shaders/SkyboxVertexShader.glsl", "shaders/SkyboxFragmentShader.glsl");
    if (!skyboxProgram->IsLinked()) return;

    sceneProgram->Use();

    // Crea el skybox
    skybox = new CGSkybox();

    // Crea la moto Tron
    motoTron = new TronLightCycle();

    // Crea la pelota
    figPelota = new CGSphere(20, 40, 4.0f);
    ballVelocity = glm::vec3(0.0f, 0.0f, 0.0f);

    // Crea la cámara en 3ª persona
    camera = new CGCamera(motoTron, figPelota, 50.0f, 5.0f);

    // Crea la escena con la moto y la pelota
    scene = new CGScene(motoTron, figPelota);

    // Crea el Framebuffer del mapa de sombras
    bool frameBufferStatus = InitShadowMap();
    if (!frameBufferStatus)
    {
        std::cerr << "Advertencia: No se pudo inicializar el Shadow Map Framebuffer" << std::endl;
    }

    // Configura el viewport y volumen de recorte
    resize(w, h);

    // Opciones globales de renderizado OpenGL
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glFrontFace(GL_CCW);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

//
// FUNCIÓN: CGModel::finalize()
//
// PROPÓSITO: Libera los recursos dinámicos del modelo 3D
//
void CGModel::finalize()
{
    delete camera;
    camera = nullptr;

    delete scene;
    scene = nullptr;

    delete skybox;
    skybox = nullptr;

    delete sceneProgram;
    sceneProgram = nullptr;

    delete skyboxProgram;
    skyboxProgram = nullptr;

    if (shadowFBO)
    {
        glDeleteFramebuffers(1, &shadowFBO);
        shadowFBO = 0;
    }
    if (depthTexId)
    {
        glDeleteTextures(1, &depthTexId);
        depthTexId = 0;
    }
}

//
// FUNCIÓN: CGModel::resize(int w, int h)
//
// PROPÓSITO: Asigna el viewport y la matriz de proyección en perspectiva
//
void CGModel::resize(int w, int h)
{
    double fov = glm::radians(15.0);
    double sin_fov = sin(fov);
    double cos_fov = cos(fov);
    if (h == 0) h = 1;
    GLfloat aspectRatio = (GLfloat)w / (GLfloat)h;
    GLfloat wHeight = (GLfloat)(sin_fov * 0.2 / cos_fov);
    GLfloat wWidth = wHeight * aspectRatio;

    wndWidth = w;
    wndHeight = h;

    glViewport(0, 0, w, h);
    projection = glm::frustum(-wWidth, wWidth, -wHeight, wHeight, 0.2f, 1000.0f);
}

//
// FUNCIÓN: CGModel::render()
//
// PROPÓSITO: Genera la imagen completa (ShadowMap + Skybox + Escena)
//
void CGModel::render()
{
    // ========================================================
    // 1. GENERACIÓN DEL SHADOW MAP
    // ========================================================
    sceneProgram->Use();

    glm::mat4 lightViewMatrix = scene->GetLightViewMatrix();
    glm::mat4 lightPerspective = glm::ortho(-600.0f, 600.0f, -600.0f, 600.0f, -600.0f, 600.0f);
    glm::mat4 lightMVP = lightPerspective * lightViewMatrix;

    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    sceneProgram->SetUniformSubroutine(GL_FRAGMENT_SHADER, "recordDepth");
    glCullFace(GL_FRONT);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.1f, 4.0f);

    glViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);
    scene->Draw(sceneProgram, lightPerspective, lightViewMatrix, lightMVP);

    glDisable(GL_POLYGON_OFFSET_FILL);

    // ========================================================
    // 2. RENDERIZADO DEL SKYBOX
    // ========================================================
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_BACK);
    glViewport(0, 0, wndWidth, wndHeight);

    glm::mat4 view = camera->ViewMatrix();
    skyboxProgram->Use();
    skybox->Draw(skyboxProgram, projection, view);

    // ========================================================
    // 3. RENDERIZADO DE LA ESCENA CON ILUMINACIÓN Y SOMBRAS
    // ========================================================
    sceneProgram->Use();
    sceneProgram->SetUniformSubroutine(GL_FRAGMENT_SHADER, "shadeWithShadow");
    sceneProgram->SetUniformI("ShadowMap", 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthTexId);

    glm::mat4 viewMatrix = camera->ViewMatrix();
    scene->Draw(sceneProgram, projection, viewMatrix, lightMVP);
}

//
// FUNCIÓN: CGModel::update()
//
// PROPÓSITO: Actualiza físicas, movimientos y colisiones
//
void CGModel::update()
{
    // Fricción del vehículo al soltar acelerador/freno
    if (motoSpeed > 0.0f) {
        motoSpeed -= friccion;
        if (motoSpeed < 0.0f) motoSpeed = 0.0f;
    }
    else if (motoSpeed < 0.0f) {
        motoSpeed += friccion;
        if (motoSpeed > 0.0f) motoSpeed = 0.0f;
    }

    // Actualiza posición de la moto
    scene->motoTron->Translate(glm::vec3(0, 0, 1) * motoSpeed);

    // Restricciones de movimiento
    ApplyConstraintsToMotoTronPosition();
    camera->UpdatePosition();
    CameraConstraints();
    ApplyConstraintsToBallPosition();
}

void CGModel::ApplyConstraintsToBallPosition()
{
    ReactToMotoCollision();

    float deltaTime = 0.016f; // ~60 FPS
    float gravity = -22.0f;

    ballVelocity.y += gravity * deltaTime;

    float frictionCoefficient = 0.98f;
    ballVelocity.x *= frictionCoefficient;
    ballVelocity.z *= frictionCoefficient;

    glm::mat4 loc = figPelota->getLocation();
    glm::vec3 pos = glm::vec3(loc[3]);
    pos.y += ballVelocity.y * (deltaTime * 3);
    pos.x += ballVelocity.x * (deltaTime * 10);
    pos.z += ballVelocity.z * (deltaTime * 10);

    float groundHeight = 0.0f;
    float ballRadius = 4.0f;

    if (pos.y < groundHeight + ballRadius) {
        pos.y = groundHeight + ballRadius;
        ballVelocity.y *= -0.8f; // Rebote vertical
    }

    float minX = -100.0f + ballRadius;
    float maxX = 100.0f - ballRadius;
    float minZ = -400.0f + ballRadius;
    float maxZ;

    if (-30 < pos.x && pos.x < 30) {
        if (pos.y < 14) { maxZ = 409.0f - ballRadius; }
        else { maxZ = 400.0f - ballRadius; }
    }
    else {
        maxZ = 400.0f - ballRadius;
    }

    if (pos.x > maxX) {
        pos.x = maxX;
        ballVelocity.x *= -0.8f;
    }
    if (pos.x < minX) {
        pos.x = minX;
        ballVelocity.x *= -0.8f;
    }
    if (pos.z > maxZ) {
        pos.z = maxZ;
        ballVelocity.z *= -0.8f;
    }
    if (pos.z < minZ) {
        pos.z = minZ;
        ballVelocity.z *= -0.8f;
    }

    loc[3] = glm::vec4(pos, 1.0f);
    figPelota->setLocation(loc);

    // Detección de gol (portería)
    if (400.0f < (pos.z - ballRadius)) {
        closeApplication(true);
    }
}

void CGModel::ReactToMotoCollision()
{
    glm::vec3 ballPos = glm::vec3(figPelota->getLocation()[3]);
    glm::mat4 motoLoc = scene->motoTron->GetLocation();
    glm::vec3 motoPos = glm::vec3(motoLoc[3]);

    float sphereRadius = 20.0f;
    glm::vec3 sphereCenter = motoPos;
    sphereCenter.y -= sphereRadius / 2.0f;

    float distance = glm::distance(ballPos, sphereCenter);

    if (distance <= sphereRadius) {
        glm::vec3 impactDirection = glm::normalize(ballPos - motoPos);
        float impactSpeed = glm::length(motoSpeed) + 0.2f;
        float transferRatio = 1.5f;
        glm::vec3 newBallVelocity = impactDirection * impactSpeed * transferRatio;

        ballVelocity += newBallVelocity;

        float groundHeight = 0.0f;
        float ballRadius = 4.0f;

        if (ballVelocity.y < groundHeight + ballRadius) {
            ballVelocity.y = groundHeight + ballRadius;
        }
    }
}

//
// FUNCIÓN: CGModel::key_pressed(int key)
//
// PROPÓSITO: Respuesta a acciones de teclado
//
void CGModel::key_pressed(int key)
{
    switch (key)
    {
    case GLFW_KEY_C:
        camera->changeTarget();
        break;
    case GLFW_KEY_ESCAPE:
        closeApplication(false);
        break;

    case GLFW_KEY_S:
        motoSpeed += acceleration;
        if (motoSpeed > maxSpeed) motoSpeed = maxSpeed;
        scene->motoTron->Translate(glm::vec3(0, 0, 1) * motoSpeed);
        break;
    case GLFW_KEY_W:
        motoSpeed -= acceleration;
        if (motoSpeed < -maxSpeed) motoSpeed = -maxSpeed;
        scene->motoTron->Translate(glm::vec3(0, 0, 1) * motoSpeed);
        break;

    case GLFW_KEY_A:
        scene->motoTron->Rotate(2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        break;
    case GLFW_KEY_D:
        scene->motoTron->Rotate(-2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        break;
    }
}

//
// FUNCIÓN: CGModel::closeApplication(bool gol)
//
// PROPÓSITO: Cierra la ventana y muestra mensaje de victoria si corresponde
//
void CGModel::closeApplication(bool gol)
{
    GLFWwindow* window = glfwGetCurrentContext();
    if (window != nullptr) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (gol) {
        std::cout << "\n========================================" << std::endl;
        std::cout << "        ¡¡¡HAS ANOTADO GOL!!!          " << std::endl;
        std::cout << "========================================\n" << std::endl;
    }
}

void CGModel::mouse_button(int button, int action)
{
}

//
// FUNCIÓN: CGModel::mouse_move(double xpos, double ypos)
//
// PROPÓSITO: Control de orientación de cámara con el ratón
//
void CGModel::mouse_move(double xpos, double ypos)
{
    static bool firstMouse = true;
    static double lastX = xpos;
    static double lastY = ypos;

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    double xoffset = xpos - lastX;
    double yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    camera->TurnRight(xoffset);
}

//
// FUNCIÓN: CGModel::CameraConstraints()
//
// PROPÓSITO: Limita la cámara al espacio de juego
//
void CGModel::CameraConstraints()
{
    glm::vec3 pos = camera->GetPosition();
    int constraint = 0;
    float groundHeight = 0.0f;

    if (pos.y < groundHeight + 1.0f) { pos.y = groundHeight + 1.0f; constraint = 1; }
    if (pos.y > groundHeight + 40.0f) { pos.y = groundHeight + 40.0f; constraint = 1; }

    float minX = -100.0f;
    float maxX = 100.0f;
    float minZ = -400.0f;
    float maxZ = 400.0f;

    if (pos.x > maxX) { pos.x = maxX; constraint = 1; }
    if (pos.x < minX) { pos.x = minX; constraint = 1; }
    if (pos.z > maxZ) { pos.z = maxZ; constraint = 1; }
    if (pos.z < minZ) { pos.z = minZ; constraint = 1; }

    if (constraint == 1)
    {
        camera->SetPosition(pos.x, pos.y, pos.z);
        camera->SetMoveStep(0.0f);
    }
}

//
// FUNCIÓN: CGModel::ApplyConstraintsToMotoTronPosition()
//
// PROPÓSITO: Limita el movimiento de la moto al espacio del campo
//
void CGModel::ApplyConstraintsToMotoTronPosition()
{
    glm::mat4 loc = scene->motoTron->GetLocation();
    glm::vec3 pos = glm::vec3(loc[3]);
    float groundHeight = 0.0f;

    if (pos.y < groundHeight + 1.0f) { pos.y = groundHeight + 1.0f; }
    if (pos.y > groundHeight + 40.0f) { pos.y = groundHeight + 40.0f; }

    float minX = -95.0f;
    float maxX = 95.0f;
    float minZ = -395.0f;
    float maxZ = 395.0f;

    if (pos.x > maxX) { pos.x = maxX; }
    if (pos.x < minX) { pos.x = minX; }
    if (pos.z > maxZ) { pos.z = maxZ; }
    if (pos.z < minZ) { pos.z = minZ; }

    loc[3] = glm::vec4(pos, 1.0f);
    scene->motoTron->SetLocation(loc);
}

//
// FUNCIÓN: CGModel::InitShadowMap()
//
// PROPÓSITO: Inicializa el FBO y textura de profundidad para mapa de sombras
//
bool CGModel::InitShadowMap()
{
    GLfloat border[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    glGenFramebuffers(1, &shadowFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

    glGenTextures(1, &depthTexId);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthTexId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, SHADOW_MAP_SIZE,
        SHADOW_MAP_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexId, 0);

    glDrawBuffer(GL_NONE);

    bool result = (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return result;
}
