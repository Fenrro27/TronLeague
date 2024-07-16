
#include "CGCamera.h"
#include "CGObject.h" // Asumiendo que CGObject es el nombre de la clase del objeto objetivo
#include "CGFigure.h"
#include <GL/glew.h>
#include <math.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
// Constructor de la clase CGCamera
CGCamera::CGCamera(CGObject* target, CGFigure* target2, GLfloat dist, GLfloat h)
    : Target(target), Target2(target2), distance(dist), height(h), yaw(+90.0f), pitch(0.0f)
{
    // Inicializa la posición de la cámara
    Pos = glm::vec3(0.0f, height, distance);
    // Calcula la dirección de la cámara hacia el objeto objetivo
    Dir = glm::normalize(Target->GetPosition() - Pos);
    // Vector hacia arriba
    Up = glm::vec3(0.0f, 1.0f, 0.0f);
    // Vector hacia la derecha
    Right = glm::normalize(glm::cross(Dir, Up));

    // Inicializa los pasos de movimiento y giro
    moveStep = 0.1f;
    turnStep = 1.0f;

    // Actualiza los vectores de la cámara
    updateCameraVectors();
}

// Método para obtener la matriz de vista de la cámara
glm::mat4 CGCamera::ViewMatrix()
{
    // Crea y retorna la matriz de vista utilizando la posición, dirección y vector hacia arriba
    glm::vec3 targetPosition = useTarget2 ? Target2->GetPosition() : Target->GetPosition();
    return glm::lookAt(Pos, targetPosition, Up);

}

// Método para establecer la posición de la cámara
void CGCamera::SetPosition(GLfloat x, GLfloat y, GLfloat z)
{
    // Establece la nueva posición
    Pos = glm::vec3(x, y, z);
    // Actualiza los vectores de la cámara
    updateCameraVectors();
}

// Método para establecer la dirección de la cámara
void CGCamera::SetDirection(GLfloat xD, GLfloat yD, GLfloat zD, GLfloat xU, GLfloat yU, GLfloat zU)
{
    // Establece la nueva dirección
    Dir = glm::normalize(glm::vec3(xD, yD, zD));
    // Mantiene el vector Up siempre hacia arriba
    Up = glm::vec3(0.0f, 1.0f, 0.0f);
    // Calcula el vector hacia la derecha
    Right = glm::normalize(glm::cross(Dir, Up));
    // Actualiza los vectores de la cámara
    updateCameraVectors();
}

// Método para establecer el paso de movimiento de la cámara
void CGCamera::SetMoveStep(GLfloat step)
{
    moveStep = step;
}

// Método para establecer el paso de giro de la cámara
void CGCamera::SetTurnStep(GLfloat step)
{
    turnStep = step;
}

// Método para obtener la posición de la cámara
glm::vec3 CGCamera::GetPosition()
{
    return Pos;
}

// Método para obtener la dirección de la cámara
glm::vec3 CGCamera::GetDirection()
{
    return Dir;
}

// Método para obtener el vector Up de la cámara
glm::vec3 CGCamera::GetUpDirection()
{
    return Up;
}

// Método para obtener el paso de movimiento de la cámara
GLfloat CGCamera::GetMoveStep()
{
    return moveStep;
}

// Método para obtener el paso de giro de la cámara
GLfloat CGCamera::GetTurnStep()
{
    return turnStep;
}

// Método para girar la cámara hacia la derecha
void CGCamera::TurnRight(double d)
{
    if (!useTarget2) {
        // Incrementa el ángulo yaw en función del paso de giro y la distancia
        yaw += turnStep * d;
        // Actualiza los vectores de la cámara
        updateCameraVectors();
    }
}

// Método para actualizar la posición de la cámara
void CGCamera::UpdatePosition()
{
    if (!useTarget2) { //Modo normal de la camara
        // Obtiene la posición del objeto objetivo
        glm::vec3 targetPos = Target->GetPosition();
        // Calcula el radio y los offsets en Y y X
        float radius = sqrt(distance * distance + height * height);
        float offsetY = height / radius;
        float offsetX = sqrt(1.0f - offsetY * offsetY);
        // Calcula la nueva posición de la cámara
        Pos.x = targetPos.x - offsetX * distance * cos(glm::radians(yaw));
        Pos.y = targetPos.y + height;
        Pos.z = targetPos.z - offsetX * distance * sin(glm::radians(yaw));
    }
    else { //Apuntado al balon de la camara
        // Obtiene las posiciones del target y target2
        glm::vec3 target1Pos = Target->GetPosition();
        glm::vec3 target2Pos = Target2->GetPosition();
        // Calcula el vector de dirección desde target2 hacia target1
        glm::vec3 direction = glm::normalize(target1Pos - target2Pos);
        // Calcula la nueva posición de la cámara, moviéndola hacia atrás en la dirección opuesta
        Pos = target1Pos + direction * (distance * 2);
        Pos.y = target1Pos.y + height*3;
    }
    // Actualiza la dirección de la cámara
    Dir = glm::normalize((useTarget2 ? Target2->GetPosition() : Target->GetPosition()) - Pos); 
    updateCameraVectors();
}

// Método para actualizar los vectores de la cámara
void CGCamera::updateCameraVectors()
{
    // Calcula el vector front
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = 0.0f; // Mantiene y en 0 para mantener la cámara paralela al suelo
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    Dir = glm::normalize(front);

    // Calcula el vector hacia la derecha
    Right = glm::normalize(glm::cross(Dir, glm::vec3(0.0f, 1.0f, 0.0f)));
    // Mantiene el vector Up siempre hacia arriba
    Up = glm::vec3(0.0f, 1.0f, 0.0f);
}


void CGCamera::changeTarget() {
    useTarget2 = !useTarget2;
    UpdatePosition();
}