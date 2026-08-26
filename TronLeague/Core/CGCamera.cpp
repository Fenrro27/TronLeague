#include "CGCamera.h"
#include "CGObject.h"
#include "CGFigure.h"
#include <GL/glew.h>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <algorithm>

//
// FUNCIÓN: CGCamera::CGCamera(...)
//
// PROPÓSITO: Constructor de la cámara en 3ª persona
//
CGCamera::CGCamera(CGObject* target, CGFigure* target2, GLfloat dist, GLfloat h)
    : Target(target), Target2(target2), baseDistance(dist), baseHeight(h)
{
    glm::vec3 targetPos = Target->GetPosition();
    currentLookAt = targetPos + glm::vec3(0.0f, 2.5f, 0.0f);
    Pos = targetPos + glm::vec3(0.0f, baseHeight, -baseDistance);
    Dir = glm::normalize(currentLookAt - Pos);
    Up = glm::vec3(0.0f, 1.0f, 0.0f);
    Right = glm::normalize(glm::cross(Dir, Up));

    moveStep = 0.1f;
    turnStep = 0.12f;
    targetBlend = 0.0f;
    orbitYaw = 0.0f;
    orbitPitch = 0.0f;

    updateCameraVectors();
}

glm::mat4 CGCamera::ViewMatrix()
{
    return glm::lookAt(Pos, currentLookAt, Up);
}

void CGCamera::SetPosition(GLfloat x, GLfloat y, GLfloat z)
{
    Pos = glm::vec3(x, y, z);
    updateCameraVectors();
}

void CGCamera::SetDirection(GLfloat xD, GLfloat yD, GLfloat zD, GLfloat xU, GLfloat yU, GLfloat zU)
{
    Dir = glm::normalize(glm::vec3(xD, yD, zD));
    Up = glm::vec3(0.0f, 1.0f, 0.0f);
    Right = glm::normalize(glm::cross(Dir, Up));
    updateCameraVectors();
}

void CGCamera::SetMoveStep(GLfloat step)
{
    moveStep = step;
}

void CGCamera::SetTurnStep(GLfloat step)
{
    turnStep = step;
}

glm::vec3 CGCamera::GetPosition()
{
    return Pos;
}

glm::vec3 CGCamera::GetDirection()
{
    return Dir;
}

glm::vec3 CGCamera::GetUpDirection()
{
    return Up;
}

GLfloat CGCamera::GetMoveStep()
{
    return moveStep;
}

GLfloat CGCamera::GetTurnStep()
{
    return turnStep;
}

void CGCamera::TurnRight(double d)
{
    if (!useTarget2) {
        orbitYaw += (float)(turnStep * d);
    }
}

//
// FUNCIÓN: CGCamera::UpdatePosition(...)
//
// PROPÓSITO: Actualiza la posición con autocentrado dinámico detrás de la moto en movimiento
//
void CGCamera::UpdatePosition(float dt, float speedRatio, float motoHeading)
{
    if (dt <= 0.0f) dt = 0.016f;
    if (dt > 0.05f) dt = 0.05f;

    // Autocentrado progresivo: si la moto se mueve, la cámara tiende a centrarse detrás
    if (speedRatio > 0.02f && !useTarget2)
    {
        float autoCenterSpeed = 2.5f + 5.0f * speedRatio;
        float centerFactor = 1.0f - std::exp(-autoCenterSpeed * dt);
        orbitYaw = glm::mix(orbitYaw, 0.0f, centerFactor);
        orbitPitch = glm::mix(orbitPitch, 0.0f, centerFactor);
    }

    // Transición suave entre el objetivo de la moto y del balón
    float targetBlendDest = useTarget2 ? 1.0f : 0.0f;
    targetBlend += (targetBlendDest - targetBlend) * (1.0f - std::exp(-8.0f * dt));

    glm::vec3 motoPos = Target->GetPosition();
    glm::vec3 ballPos = Target2->GetPosition();

    // Punto de mira objetivo con suavizado
    glm::vec3 desiredLookAt = glm::mix(motoPos + glm::vec3(0.0f, 2.5f, 0.0f), ballPos, targetBlend);
    float lookAtSmoothing = 1.0f - std::exp(-14.0f * dt);
    currentLookAt = glm::mix(currentLookAt, desiredLookAt, lookAtSmoothing);

    // Ajuste dinámico de distancia y altura según la velocidad
    float dynamicDist = baseDistance + speedRatio * 8.0f;
    float dynamicHeight = baseHeight + speedRatio * 2.0f;

    glm::vec3 desiredPos;
    if (targetBlend < 0.5f)
    {
        // Vector de avance de la moto (con órbita adicional si se usa el ratón)
        float radHeading = glm::radians(motoHeading + orbitYaw);
        glm::vec3 forwardVec = glm::vec3(std::sin(radHeading), 0.0f, std::cos(radHeading));

        // La cámara se sitúa exactamente detrás del morro de la moto
        desiredPos = motoPos - forwardVec * dynamicDist;
        desiredPos.y = motoPos.y + dynamicHeight;
    }
    else
    {
        // Modo 2: Cámara enfocada en el balón
        glm::vec3 dirToBall = motoPos - ballPos;
        float distToBall = glm::length(dirToBall);
        glm::vec3 dirNorm = (distToBall > 0.001f) ? (dirToBall / distToBall) : glm::vec3(0.0f, 0.0f, 1.0f);

        desiredPos = motoPos + dirNorm * (baseDistance * 1.3f);
        desiredPos.y = motoPos.y + baseHeight * 1.8f;
    }

    // Amortiguación exponencial de la posición de la cámara (Spring-Damper)
    float posSmoothing = 1.0f - std::exp(-10.0f * dt);
    Pos = glm::mix(Pos, desiredPos, posSmoothing);

    // Límite inferior para evitar que la cámara atraviese el suelo
    if (Pos.y < 1.5f) Pos.y = 1.5f;

    Dir = glm::normalize(currentLookAt - Pos);
    updateCameraVectors();
}

void CGCamera::updateCameraVectors()
{
    Right = glm::normalize(glm::cross(Dir, glm::vec3(0.0f, 1.0f, 0.0f)));
    Up = glm::vec3(0.0f, 1.0f, 0.0f);
}

void CGCamera::changeTarget()
{
    useTarget2 = !useTarget2;
}
