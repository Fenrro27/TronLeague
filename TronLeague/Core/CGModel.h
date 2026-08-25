#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "CGShaderProgram.h"
#include "CGScene.h"
#include "CGCamera.h"
#include "CGSkybox.h"
#include "TronLightCycle.h"
#include "CGSphere.h"

//
// CLASE: CGModel
//
// DESCRIPCIÓN: Controlador y modelo lógico del juego: físicas, colisiones,
//              iluminación con mapas de sombras y gestión de entrada.
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

	glm::vec3 ballVelocity;  // Velocidad de la pelota

	GLsizei wndWidth;
	GLsizei wndHeight;
	GLuint shadowFBO;
	GLuint depthTexId;
	static const GLsizei SHADOW_MAP_SIZE = 2048;
	bool InitShadowMap();

	void CameraConstraints();
	void closeApplication(bool gol);

	// Colisión moto-pelota
	void ReactToMotoCollision();

	// Restricciones de la moto y de la pelota
	void ApplyConstraintsToMotoTronPosition();
	void ApplyConstraintsToBallPosition();

	// Velocidad y aceleración del motoTron
	float motoSpeed = 0.0f;
	float maxSpeed = 2.0f;
	float acceleration = 0.4f;
	float friccion = 0.10f;
};
