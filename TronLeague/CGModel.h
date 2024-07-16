
#pragma once

#include <GL/glew.h>
#include "CGShaderProgram.h"
#include "CGScene.h"
#include "CGCamera.h"
#include "CGSkybox.h"
#include "TronLightCycle.h"
#include "CGSphere.h"

//
// CLASE: CGModel
//
// DESCRIPCIÓN: Construye un modelo con un programa gráfico que muestra una escena con varias
//              figuras geométricas, incluyendo texturas e iluminación
//
class CGModel
{
public:

	void initialize(int w, int h);
	void finalize();
	void render();
	void update();
	void key_pressed(int key);
	void mouse_button(int button, int action);
	void mouse_move(double xpos, double ypos);
	void resize(int w, int h);

private:
	CGShaderProgram* sceneProgram;
	CGShaderProgram* skyboxProgram;
	CGScene* scene;
	CGCamera* camera;
	CGSkybox* skybox;
	glm::mat4 projection;

	CGObject* motoTron;
	CGFigure* figPelota;
	glm::vec3 directionVectorPelotaMov;

	glm::vec3 ballVelocity;  // Nueva variable para almacenar la velocidad de la pelota



	GLsizei wndWidth;
	GLsizei wndHeight;
	GLuint shadowFBO;
	GLuint depthTexId;
	bool InitShadowMap();

	void CameraConstraints();
	void closeApplication(bool gol);

	// Colision moto pelota
	void ReactToMotoCollision();

	// Restricciones de la moto y de la pelota
	void ApplyConstraintsToMotoTronPosition();
	void ApplyConstraintsToBallPosition();

	// Declaraciones adicionales para la velocidad y aceleración del motoTron
	float motoSpeed = 0.0f; // Velocidad 
	float maxSpeed = 2.0f; // Velocidad máxima
	float acceleration = 0.4f; // Aceleración
	float friccion = 0.10f; //frinccion del suelo

};