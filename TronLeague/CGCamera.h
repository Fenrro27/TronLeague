
#ifndef CGCAMERA_H
#define CGCAMERA_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


class CGObject; // Declaración adelantada de la clase CGObject
class CGFigure; // Declaración adelantada de la clase CGFigure

class CGCamera
{
public:
    // Constructor
    CGCamera(CGObject* target, CGFigure* target2, GLfloat distance, GLfloat height);

    // Funciones para obtener la matriz de vista
    glm::mat4 ViewMatrix();

    // Funciones para establecer la posición de la cámara
    void SetPosition(GLfloat x, GLfloat y, GLfloat z);

    // Funciones para establecer la dirección y pasos de movimiento/giro
    void SetDirection(GLfloat xD, GLfloat yD, GLfloat zD, GLfloat xU, GLfloat yU, GLfloat zU);
    void SetMoveStep(GLfloat step);
    void SetTurnStep(GLfloat step);

    // Funciones para obtener la posición, dirección y pasos de movimiento/giro
    glm::vec3 GetPosition();
    glm::vec3 GetDirection();
    glm::vec3 GetUpDirection();
    GLfloat GetMoveStep();
    GLfloat GetTurnStep();

    // Función para actualizar la posición de la cámara en tercera persona
    void UpdatePosition();

    // Funciones de rotación
    void TurnRight(double d);

    void changeTarget(); //Metodo para cambiar al objetivo al que se apunta

private:

    bool useTarget2= false;
    // Atributos
    glm::vec3 Pos;     // Posición de la cámara
    glm::vec3 Dir;     // Dirección de la cámara
    glm::vec3 Up;      // Vector de dirección "arriba" de la cámara
    glm::vec3 Right;   // Vector de dirección "derecha" de la cámara

    GLfloat moveStep;  // Tamaño del paso de movimiento
    GLfloat turnStep;  // Tamaño del paso de giro

    GLfloat distance;  // Distancia de la cámara al objetivo
    GLfloat height;    // Altura de la cámara desde el objetivo

    GLfloat yaw;       // Ángulo de rotación en el eje Y
    GLfloat pitch;     // Ángulo de rotación en el eje X

    CGObject* Target;  // Puntero al objeto que la cámara siempre apunta
    CGFigure* Target2;

    // Función para actualizar los vectores de la cámara
    void updateCameraVectors();
};

#endif // CGCAMERA_H