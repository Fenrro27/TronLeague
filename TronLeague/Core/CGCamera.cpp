#include "CGCamera.h"
#include "CGObject.h"
#include "CGFigure.h"
#include <GL/glew.h>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

//
// FUNCIÓN: CGCamera::CGCamera(...)
//
// PROPÓSITO: Constructor de la cámara en 3ª persona
//
CGCamera::CGCamera(CGObject* target, CGFigure* target2, GLfloat dist, GLfloat h)
    : Target(target), Target2(target2), distance(dist), height(h), yaw(+90.0f), pitch(0.0f)
{
    Pos = glm::vec3(0.0f, height, distance);
    Dir = glm::normalize(Target->GetPosition() - Pos);
    Up = glm::vec3(0.0f, 1.0f, 0.0f);
    Right = glm::normalize(glm::cross(Dir, Up));

    moveStep = 0.1f;
    turnStep = 1.0f;

    updateCameraVectors();
}

glm::mat4 CGCamera::ViewMatrix()
{
    glm::vec3 targetPosition = useTarget2 ? Target2->GetPosition() : Target->GetPosition();
    return glm::lookAt(Pos, targetPosition, Up);
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
        yaw += (GLfloat)(turnStep * d);
        updateCameraVectors();
    }
}

void CGCamera::UpdatePosition()
{
    if (!useTarget2) {
        glm::vec3 targetPos = Target->GetPosition();
        float radius = std::sqrt(distance * distance + height * height);
        float offsetY = height / radius;
        float offsetX = std::sqrt(1.0f - offsetY * offsetY);

        Pos.x = targetPos.x - offsetX * distance * std::cos(glm::radians(yaw));
        Pos.y = targetPos.y + height;
        Pos.z = targetPos.z - offsetX * distance * std::sin(glm::radians(yaw));
    }
    else {
        glm::vec3 target1Pos = Target->GetPosition();
        glm::vec3 target2Pos = Target2->GetPosition();
        glm::vec3 direction = glm::normalize(target1Pos - target2Pos);

        Pos = target1Pos + direction * (distance * 2);
        Pos.y = target1Pos.y + height * 3;
    }

    Dir = glm::normalize((useTarget2 ? Target2->GetPosition() : Target->GetPosition()) - Pos); 
    updateCameraVectors();
}

void CGCamera::updateCameraVectors()
{
    glm::vec3 front;
    front.x = std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch));
    front.y = 0.0f;
    front.z = std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch));
    Dir = glm::normalize(front);

    Right = glm::normalize(glm::cross(Dir, glm::vec3(0.0f, 1.0f, 0.0f)));
    Up = glm::vec3(0.0f, 1.0f, 0.0f);
}

void CGCamera::changeTarget()
{
    useTarget2 = !useTarget2;
    UpdatePosition();
}
