
#include "CGModel.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>
#include "CGCamera.h"
#include "CGScene.h"
#include "resource.h"

#include "iostream"

//
// FUNCIÓN: CGModel::initialize(int, int)
//
// PROPÓSITO: Initializa el modelo 3D
//
void CGModel::initialize(int w, int h)
{
    // Crea el programa gráfico para la escena
    // program = new CGShaderProgram(IDR_SHADER1, IDR_SHADER2, -1, -1, -1);
    sceneProgram = new CGShaderProgram("shaders/VertexShader.glsl", "shaders/FragmentShader.glsl", NULL, NULL, NULL);
    if (sceneProgram->IsLinked() == GL_FALSE) return;

    // Crea el programa gráfico para el entorno
    skyboxProgram = new CGShaderProgram("shaders/SkyboxVertexShader.glsl", "shaders/SkyboxFragmentShader.glsl", NULL, NULL, NULL);
    // skyboxProgram = new CGShaderProgram(IDR_SHADER3, IDR_SHADER4, -1, -1, -1);
    if (skyboxProgram->IsLinked() == GL_FALSE) return;


    sceneProgram->Use();

    // Crea el skybox
    skybox = new CGSkybox();

    //Crea la moto
    motoTron = new TronLightCycle();

    // creamos la pelota
    figPelota = new CGSphere(20, 40, 4.0f);
    // Inicializa la velocidad de la pelota
    ballVelocity = glm::vec3(0.0f, 0.0f, 0.0f);


    // Crea la cámara
    camera = new CGCamera(motoTron,figPelota , 50.0f, 5.0f);

    // Crea la escena
    scene = new CGScene(motoTron, figPelota);


    // Crea el Framebuffer de la sombra
    bool frameBufferStatus = InitShadowMap();
    if (!frameBufferStatus) return;

    // Asigna el viewport y el clipping volume
    resize(w, h);

    // Opciones de dibujo
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
// PROPÓSITO: Libera los recursos del modelo 3D
//
void CGModel::finalize()
{

    delete camera;
    delete scene;
    delete skybox;
    delete sceneProgram;
    delete skyboxProgram;
    // delete motoTron;
}

//
// FUNCIÓN: CGModel::resize(int w, int h)
//
// PROPÓSITO: Asigna el viewport y el clipping volume
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
    projection = glm::frustum(-wWidth, wWidth, -wHeight, wHeight, 0.2f, 1000.0f); //El ultimo numero es la distancia de renderizado
}

//
// FUNCIÓN: CGModel::render()
//
// PROPÓSITO: Genera la imagen
//
void CGModel::render()
{
    //*********************************************************//
  //                  Genera el ShadowMap                    //
  //*********************************************************//

  // Activa el programa de la escena
    sceneProgram->Use();

    // Asigna las matrices Viewport, View y Projection de la luz.
    glm::mat4 lightViewMatrix = scene->GetLightViewMatrix();
    glm::mat4 lightPerspective = glm::ortho(-600.0f, 600.0f, -600.0f, 600.0f, -600.0f, 600.0f);
    glm::mat4 lightMVP = lightPerspective * lightViewMatrix;

    // Activa el framebuffer de la sombra
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

    // Limpia la información de profundidad
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Selecciona la subrutina recordDepth
    sceneProgram->SetUniformSubroutine(GL_FRAGMENT_SHADER, "recordDepth");

    // Activa front-face culling
    glCullFace(GL_FRONT);

    // Configura y habilita el desplazamiento de polígonos
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.1f, 4.0f);


    //Asigna el viewport
    glViewport(0, 0, 12048, 12048);

    // Dibuja la escena
    scene->Draw(sceneProgram, lightPerspective, lightViewMatrix, lightMVP);

    // Desactiva el desplazamiento de polígonos después de dibujar el shadow map
    glDisable(GL_POLYGON_OFFSET_FILL);

    //*********************************************************//
    //                  Dibuja el skybox                       //
    //*********************************************************//

    // Activa el framebuffer de la imagen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Limpia el framebuffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Activa back-face culling
    glCullFace(GL_BACK);

    // Asigna el viewport
    glViewport(0, 0, wndWidth, wndHeight);

    // Dibuja el skybox
    glm::mat4 view = camera->ViewMatrix();
    skyboxProgram->Use();
    skybox->Draw(skyboxProgram, projection, view);

    //*********************************************************//
    //                  Dibuja la escena                       //
    //*********************************************************//

    // Activa el programa de la escena
    sceneProgram->Use();

    // Selecciona la subrutina shadeWithShadow
    sceneProgram->SetUniformSubroutine(GL_FRAGMENT_SHADER, "shadeWithShadow");
    sceneProgram->SetUniformI("ShadowMap", 1);

    // Dibuja la escena
    glm::mat4 viewMatrix = camera->ViewMatrix();
    scene->Draw(sceneProgram, projection, viewMatrix, lightMVP);

}

//
// FUNCIÓN: CGModel::update()
//
// PROPÓSITO: Anima la escena
//
void CGModel::update()
{
    // Aplica la fricción/desaceleración al motoTron cuando no se están presionando las teclas de aceleración o freno
    if (motoSpeed > 0.0f) {
        motoSpeed -= friccion;
        if (motoSpeed < 0.0f) motoSpeed = 0.0f; // Asegúrate de que la velocidad no sea negativa
    }
    else if (motoSpeed < 0.0f) {
        motoSpeed += friccion;
        if (motoSpeed > 0.0f) motoSpeed = 0.0f; // Asegúrate de que la velocidad no sea positiva
    }

    // Actualiza la posición y dirección del motoTron en base a su velocidad
    scene->motoTron->Translate(glm::vec3(0, 0, 1) * motoSpeed);

    // Aplica restricciones a la posición del motoTron
    ApplyConstraintsToMotoTronPosition();

    // Calcular la nueva posición de la cámara
    camera->UpdatePosition();

    CameraConstraints();

    ApplyConstraintsToBallPosition();
}

void CGModel::ApplyConstraintsToBallPosition()
{
    // Calculamos la posicion y velocidad de la pelota
    //Reaccionamos a la colision
    ReactToMotoCollision();

    // Actualiza la posición de la pelota aplicando gravedad
    float deltaTime = 0.016f; // Suponiendo 60 FPS (1/60)
    float gravity = -22.0f; // Aceleración debida a la gravedad

    // Actualiza la velocidad vertical de la pelota
    ballVelocity.y += gravity * deltaTime;

    // Aplica fricción a la pelota en los ejes X y Z
    float frictionCoefficient = 0.98f;
    ballVelocity.x *= frictionCoefficient;
    ballVelocity.z *= frictionCoefficient;

    // Actualiza la posición de la pelota
    glm::mat4 loc = figPelota->getLocation();
    glm::vec3 pos = glm::vec3(loc[3]);
    pos.y += ballVelocity.y * (deltaTime * 3); // Hacemos que la velocidad aumente
    pos.x += ballVelocity.x * (deltaTime * 10); // Hacemos que la velocidad aumente
    pos.z += ballVelocity.z * (deltaTime * 10); // Hacemos que la velocidad aumente

    // Nos aseguramos de que la pelota no pase por debajo del suelo
    float groundHeight = 0.0f; // Altura del suelo
    float ballRadius = 4.0f; // Radio de la pelota

    if (pos.y < groundHeight + ballRadius) { // Ajustado según el radio de la pelota
        pos.y = groundHeight + ballRadius;
        ballVelocity.y *= -0.8f; // Bote vertical
    }

    // Restricciones laterales
    float minX = -100.0f + ballRadius; // Límite izquierdo del suelo
    float maxX = 100.0f - ballRadius;  // Límite derecho del suelo
    float minZ = -400.0f + ballRadius; // Límite trasero del suelo
    float maxZ;

    if (-30 < pos.x && pos.x < 30 ) { //Si esta dentro de la porteria
        if (pos.y < 14) {maxZ = 409.0f - ballRadius;}  // Límite frontal del suelo
        else {maxZ = 400.0f - ballRadius;}  // Límite frontal del suelo
    }
    else {maxZ = 400.0f - ballRadius;}  // Límite frontal del suelo
 
    if (pos.x > maxX) {
        pos.x = maxX;
        ballVelocity.x *= -0.8f; // Bote lateral derecho
    }
    if (pos.x < minX) {
        pos.x = minX;
        ballVelocity.x *= -0.8f; // Bote lateral izquierdo
    }
    if (pos.z > maxZ) {
        pos.z = maxZ;
        ballVelocity.z *= -0.8f; // Bote trasero
    }
    if (pos.z < minZ) {
        pos.z = minZ;
        ballVelocity.z *= -0.8f; // Bote frontal
    }

    loc[3] = glm::vec4(pos, 1.0f);
    figPelota->setLocation(loc);

    // Comprobamos si la pelota ha anotado gol

    if ( 400.0f < (pos.z - ballRadius)) {
        closeApplication(true);
        
    }


}



void CGModel::ReactToMotoCollision()
{
    // Obtener la posición de la pelota
    glm::vec3 ballPos = glm::vec3(figPelota->getLocation()[3]);

    // Obtener la posición y dimensiones de la moto
    glm::mat4 motoLoc = scene->motoTron->GetLocation();
    glm::vec3 motoPos = glm::vec3(motoLoc[3]);

    // Definir las dimensiones de la esfera
    float sphereRadius = 20.0f;  // Radio de la esfera (ajustado para representar la moto)

    // Calcular el centro de la esfera (ajustado para estar parcialmente bajo tierra)
    glm::vec3 sphereCenter = motoPos;
    // Ajustar el centro de la esfera para que esté parcialmente bajo tierra
    sphereCenter.y -= sphereRadius / 2.0f;  

    // Calcular la distancia entre la pelota y el centro de la esfera
    float distance = glm::distance(ballPos, sphereCenter);

    // Verificar si la distancia es menor o igual al radio de la esfera
    if (distance <= sphereRadius) {
        // Hay colisión

        // Calcular la dirección del impacto
        glm::vec3 impactDirection = glm::normalize(ballPos - motoPos);

        // Transferir velocidad de la moto a la pelota
        float impactSpeed = glm::length(motoSpeed)+0.2f; // Añadimos una velocidad minima
        float transferRatio = 1.5f; // Ratio de transferencia de velocidad
        glm::vec3 newBallVelocity = impactDirection * impactSpeed * transferRatio;

        // Actualizar la velocidad de la pelota
        ballVelocity += newBallVelocity; // Añadir la nueva velocidad a la velocidad actual

        // Asegúrate de que la pelota no pase por debajo del suelo
        float groundHeight = 0.0f; // Altura del suelo
        float ballRadius = 4.0f; // Radio de la pelota

        if (ballVelocity.y < groundHeight + ballRadius) { // Ajuste según el radio de la pelota
            ballVelocity.y = groundHeight + ballRadius;
        }
    }
}




//
// FUNCIÓN: CGModel::key_pressed(int)
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
    case GLFW_KEY_ESCAPE: // Cerramos la app
        closeApplication(false);
        break;

    case GLFW_KEY_S: //Acelerar
        motoSpeed += acceleration; // Aumentar la velocidad hacia adelante
        if (motoSpeed > maxSpeed) motoSpeed = maxSpeed; // Limitar la velocidad máxima

        scene->motoTron->Translate(glm::vec3(0, 0, 1) * motoSpeed);
        break;
    case GLFW_KEY_W: //Frenar
        motoSpeed -= acceleration; // Aumentar la velocidad hacia atrás
        if (motoSpeed < -maxSpeed) motoSpeed = -maxSpeed; // Limitar la velocidad máxima hacia atrás

        scene->motoTron->Translate(glm::vec3(0, 0, 1) * motoSpeed);
        break;

    case GLFW_KEY_A:
        scene->motoTron->Rotate(2.0, glm::vec3(0.0f, 1.0f, 0.0f));
        break;
    case GLFW_KEY_D:
        scene->motoTron->Rotate(-2.0, glm::vec3(0.0f, 1.0f, 0.0f));
        break;
    }
}

// Cerramos la aplicacion
void CGModel::closeApplication(bool gol)
{
    // Obtener la referencia a la ventana de la aplicación
    GLFWwindow* window = glfwGetCurrentContext();
    if (window != nullptr) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
    if (gol) {
        std::cout << "Has anotado gol!!!" << std::endl;
    }

}


//
//  FUNCIÓN: CGModel:::mouse_button(int button, int action)
//
//  PROPÓSITO: Respuesta del modelo a un click del ratón.
//
void CGModel::mouse_button(int button, int action)
{

}

//
//  FUNCIÓN: CGModel::mouse_move(double xpos, double ypos)
//
//  PROPÓSITO: Respuesta del modelo a un movimiento del ratón.
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
    double yoffset = lastY - ypos; // Reverso porque la coordenada y va de abajo hacia arriba
    lastX = xpos;
    lastY = ypos;

    const float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    camera->TurnRight(xoffset);
}



//
//  FUNCIÓN: CGModel::CameraConstraints()
//
//  PROPÓSITO: Limita el movimiento de la cámara a una cierta zona
//
void CGModel::CameraConstraints()
{
    glm::vec3 pos = camera->GetPosition();
    int constraint = 0;
    // Limitaciones del suelo
    float groundHeight = 0.0f; // Altura del suelo

    if (pos.y < groundHeight + 1.0f) { pos.y = groundHeight + 1.0f; constraint = 1; } // Altura mínima de la cámara
    if (pos.y > groundHeight + 40.0f) { pos.y = groundHeight + 40.0f; constraint = 1; } // Altura máxima de la cámara

    // Restricciones laterales
    float minX = -100.0f; // Límite izquierdo del suelo
    float maxX = 100.0f;  // Límite derecho del suelo
    float minZ = -400.0f; // Límite frontal del suelo
    float maxZ = 400.0f;  // Límite trasero del suelo

    if (pos.x > maxX) { pos.x = maxX; constraint = 1; } // Límite derecho
    if (pos.x < minX) { pos.x = minX; constraint = 1; } // Límite izquierdo
    if (pos.z > maxZ) { pos.z = maxZ; constraint = 1; } // Límite trasero
    if (pos.z < minZ) { pos.z = minZ; constraint = 1; } // Límite frontal

    if (constraint == 1)
    {
        camera->SetPosition(pos.x, pos.y, pos.z);
        camera->SetMoveStep(0.0f);
    }
}

// Implementación de la función para aplicar restricciones a la posición del motoTron
void CGModel::ApplyConstraintsToMotoTronPosition()
{
    glm::mat4 loc = scene->motoTron->GetLocation();
    // Obtener la posición actual de la motoTron
    glm::vec3 pos = glm::vec3(loc[3]); // La posición está en la columna 4 de la matriz de modelo
    // Limitaciones del suelo
    float groundHeight = 0.0f; // Altura del suelo
    if (pos.y < groundHeight + 1.0f) {pos.y = groundHeight + 1.0f;} // Altura mínima de la moto
    if (pos.y > groundHeight + 40.0f) {pos.y = groundHeight + 40.0f;} // Altura máxima de la moto
    // Restricciones laterales
    float minX = -95.0f; // Límite izquierdo
    float maxX = 95.0f;  // Límite derecho
    float minZ = -395.0f; // Límite frontal
    float maxZ = 395.0f;  // Límite trasero
    if (pos.x > maxX) {pos.x = maxX;} // Límite derecho
    if (pos.x < minX) {pos.x = minX;} // Límite izquierdo
    if (pos.z > maxZ) {pos.z = maxZ;} // Límite trasero
    if (pos.z < minZ) {pos.z = minZ;} // Límite frontal
    // Actualizar la ubicación de la motoTron respetando las restricciones
    loc[3] = glm::vec4(pos, 1.0f); // Actualizar la posición en la matriz de modelo
    scene->motoTron->SetLocation(loc);
}



//
// FUNCIÓN: CGModel::InitShadowMap()
//
// PROPÓSITO: Inicializa el FBO para almacenar la textura de sombre
//
bool CGModel::InitShadowMap()
{
    GLfloat border[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLsizei shadowMapWidth = 12048;
    GLsizei shadowMapHeight = 12048;


    glGenFramebuffers(1, &shadowFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);

    glGenTextures(1, &depthTexId);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthTexId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, shadowMapWidth,
        shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexId, 0);

    glDrawBuffer(GL_NONE);

    bool result = true;
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        result = false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return result;
}
