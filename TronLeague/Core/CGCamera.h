#ifndef CGCAMERA_H
#define CGCAMERA_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class CGObject;
class CGFigure;

//
// CLASE: CGCamera
//
// DESCRIPCIÓN: Gestiona la cámara en tercera persona orientada a la moto y al balón
//
class CGCamera
{
public:
    CGCamera(CGObject* target, CGFigure* target2, GLfloat distance, GLfloat height);

    glm::mat4 ViewMatrix();
    void SetPosition(GLfloat x, GLfloat y, GLfloat z);
    void SetDirection(GLfloat xD, GLfloat yD, GLfloat zD, GLfloat xU, GLfloat yU, GLfloat zU);
    void SetMoveStep(GLfloat step);
    void SetTurnStep(GLfloat step);

    glm::vec3 GetPosition();
    glm::vec3 GetDirection();
    glm::vec3 GetUpDirection();
    GLfloat GetMoveStep();
    GLfloat GetTurnStep();

    void UpdatePosition();
    void TurnRight(double d);
    void changeTarget();

private:
    bool useTarget2 = false;

    glm::vec3 Pos;
    glm::vec3 Dir;
    glm::vec3 Up;
    glm::vec3 Right;

    GLfloat moveStep;
    GLfloat turnStep;
    GLfloat distance;
    GLfloat height;

    GLfloat yaw;
    GLfloat pitch;

    CGObject* Target;
    CGFigure* Target2;

    void updateCameraVectors();
};

#endif // CGCAMERA_H
