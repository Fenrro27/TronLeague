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
// DESCRIPCIÓN: Cámara cinematográfica en 3ª persona con autocentrado dinámico tras la moto,
//              amortiguación Spring-Damper y cambio suave de objetivo.
//
class CGCamera
{
public:
    CGCamera(CGObject* target, CGFigure* target2, GLfloat distance = 22.0f, GLfloat height = 6.5f);

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

    void UpdatePosition(float dt = 0.016f, float speedRatio = 0.0f, float motoHeading = 0.0f);
    void TurnRight(double d);
    void changeTarget();
    bool isTargetBall() const { return useTarget2; }

private:
    bool useTarget2 = false;
    float targetBlend = 0.0f;

    glm::vec3 Pos;
    glm::vec3 currentLookAt;
    glm::vec3 Dir;
    glm::vec3 Up;
    glm::vec3 Right;

    GLfloat moveStep;
    GLfloat turnStep;
    GLfloat baseDistance;
    GLfloat baseHeight;

    // Desplazamiento orbital del ratón
    float orbitYaw = 0.0f;
    float orbitPitch = 0.0f;

    CGObject* Target;
    CGFigure* Target2;

    void updateCameraVectors();
};

#endif // CGCAMERA_H
