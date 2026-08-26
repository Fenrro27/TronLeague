#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
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
// DESCRIPCIÓN: Controlador y motor físico del juego:
//              - Físicas vehiculares de la moto Tron (aceleración, fricción, giro e inclinación lateral)
//              - Físicas del balón (gravedad, aerodinámica, rebotes elásticos y rotación 3D)
//              - Colisión e impulso moto-balón
//              - Cámara con perspectiva 3D realista y autocentrado
//              - Marcador e interfaz gráfica Dear ImGui
//
class CGModel
{
public:
	void initialize(int w, int h);
	void finalize();
	void render();
	void renderUI();
	void update(float dt = 0.016f, GLFWwindow* window = nullptr);
	void key_pressed(int key);
	void mouse_button(int button, int action);
	void mouse_move(double xpos, double ypos);
	void resize(int w, int h);

	void resetPositions();
	int getScore() const { return score; }

private:
	CGShaderProgram* sceneProgram;
	CGShaderProgram* skyboxProgram;
	CGScene* scene;
	CGCamera* camera;
	CGSkybox* skybox;
	glm::mat4 projection;

	CGObject* motoTron;
	CGFigure* figPelota;

	// Variables físicas de la moto Tron
	glm::vec3 motoPos = glm::vec3(0.0f, 3.2f, -80.0f);
	float motoHeading = 0.0f;  // 0 grados = Orientada hacia adelante (+Z, hacia el balón y la portería)
	float motoRoll = 0.0f;     // Ángulo de inclinación lateral en curvas (eje Z)
	float motoVelocity = 0.0f; // Velocidad longitudinal (unidades/s)
	const float maxForwardSpeed = 55.0f;
	const float maxReverseSpeed = -20.0f;

	// Variables físicas del balón
	glm::vec3 ballPos = glm::vec3(0.0f, 10.0f, 40.0f);
	glm::vec3 ballVelocity = glm::vec3(0.0f);
	glm::mat4 ballRotation = glm::mat4(1.0f);
	const float ballRadius = 5.0f;

	// Puntuación y UI
	int score = 0;
	float goalCelebrationTimer = 0.0f;

	GLsizei wndWidth;
	GLsizei wndHeight;
	GLuint shadowFBO;
	GLuint depthTexId;
	static const GLsizei SHADOW_MAP_SIZE = 2048;
	bool InitShadowMap();

	void CameraConstraints();
	void closeApplication(bool gol);

	// Procesamiento de colisiones y límites
	void HandleMotoBallCollision();
	void ApplyConstraintsToMoto();
	void ApplyConstraintsToBall(float dt);
};
