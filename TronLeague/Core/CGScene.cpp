#include "CGScene.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "CGShaderProgram.h"
#include "CGFigure.h"
#include "CGLight.h"
#include "CGMaterial.h"
#include "CGCube.h"
#include "CGSphere.h"
#include "CGGround.h"
#include "TronLightCycle.h"
#include "Porteria.h"

//
// FUNCIÓN: CGScene::CGScene()
//
// PROPÓSITO: Construye el objeto que representa la escena
//
CGScene::CGScene(CGObject* moto, CGFigure* pelota)
{
    glm::vec3 Ldir = glm::vec3(1.0f, -0.8f, -1.0f);
    Ldir = glm::normalize(Ldir);
    light = new CGLight();
    light->SetLightDirection(Ldir);
    light->SetAmbientLight(glm::vec3(0.3f, 0.3f, 0.3f));
    light->SetDifusseLight(glm::vec3(0.85f, 0.85f, 0.85f));
    light->SetSpecularLight(glm::vec3(1.0f, 1.0f, 1.0f));

    matground = new CGMaterial();
    matground->SetAmbientReflect(1.0f, 1.0f, 1.0f);
    matground->SetDifusseReflect(1.0f, 1.0f, 1.0f);
    matground->SetSpecularReflect(0.8f, 0.8f, 0.8f);
    matground->SetShininess(16.0f);
    matground->InitTexture("textures/Stylized_Stone_Floor_005_basecolor.jpg");

    ground = new CGGround(100.0f, 415.0f);
    ground->SetMaterial(matground);

    // Muros
    matMuros = new CGMaterial();
    matMuros->SetAmbientReflect(1.0f, 1.0f, 1.0f);
    matMuros->SetDifusseReflect(1.0f, 1.0f, 1.0f);
    matMuros->SetSpecularReflect(0.8f, 0.8f, 0.8f);
    matMuros->SetShininess(16.0f);
    matMuros->InitTexture("textures/Tiles_048_basecolor.jpg");

    float wallHeight = 10.0f;
    float wallThickness = 0.01f;
    float groundWidth = 100.0f;
    float groundLength = 400.0f;
    float gateWidth = 30.0f;

    // Muro trasero
    figMuroDetras = new CGCube(groundWidth, wallHeight, wallThickness, 1, 7);
    figMuroDetras->SetMaterial(matMuros);
    figMuroDetras->Translate(glm::vec3(0.0f, wallHeight / 2, -400));
    figMuroDetras->Rotate(180, glm::vec3(0.0f, 1.0f, 0.0f));

    // Muro delantero izquierdo
    figMuroDelanteIzquierdo = new CGCube((groundWidth / 2) - (gateWidth / 2), wallHeight, wallThickness, 1, 1);
    figMuroDelanteIzquierdo->SetMaterial(matMuros);
    figMuroDelanteIzquierdo->Translate(glm::vec3((groundWidth / 2) + (gateWidth / 2), wallHeight / 2, 400));

    // Muro delantero derecho
    figMuroDelanteDerecho = new CGCube((groundWidth / 2) - (gateWidth / 2), wallHeight, wallThickness, 1, 1);
    figMuroDelanteDerecho->SetMaterial(matMuros);
    figMuroDelanteDerecho->Translate(glm::vec3(-(groundWidth / 2) - (gateWidth / 2), wallHeight / 2, 400));

    // Muro izquierdo
    figMuroIzquierdo = new CGCube(groundLength, wallHeight, wallThickness, 1, 15);
    figMuroIzquierdo->SetMaterial(matMuros);
    figMuroIzquierdo->Translate(glm::vec3(groundWidth, wallHeight / 2, 0.0f));
    figMuroIzquierdo->Rotate(90, glm::vec3(0.0f, 1.0f, 0.0f));

    // Muro derecho
    figMuroDerecho = new CGCube(groundLength, wallHeight, wallThickness, 1, 15);
    figMuroDerecho->SetMaterial(matMuros);
    figMuroDerecho->Translate(glm::vec3(-groundWidth, wallHeight / 2, 0.0f));
    figMuroDerecho->Rotate(270.0f, glm::vec3(0.0f, 1.0f, 0.0f));

    // Portería
    matPorteria = new CGMaterial();
    matPorteria->SetAmbientReflect(1.0f, 1.0f, 1.0f);
    matPorteria->SetDifusseReflect(1.0f, 1.0f, 1.0f);
    matPorteria->SetSpecularReflect(0.8f, 0.8f, 0.8f);
    matPorteria->SetShininess(16.0f);
    matPorteria->InitTexture("textures/goal.jpg");

    figPorteria = new Porteria(gateWidth, wallHeight + 4, 10.0f);
    figPorteria->SetMaterial(matPorteria);
    figPorteria->Translate(glm::vec3(0.0f, (wallHeight / 2) + 5, 409));
    figPorteria->Rotate(180, glm::vec3(0.0f, 1.0f, 0.0f));

    // Pelota
    matPelota = new CGMaterial();
    matPelota->SetAmbientReflect(1.0f, 1.0f, 1.0f);
    matPelota->SetDifusseReflect(1.0f, 1.0f, 1.0f);
    matPelota->SetSpecularReflect(0.8f, 0.8f, 0.8f);
    matPelota->SetShininess(16.0f);
    matPelota->InitTexture("textures/ball4.png");

    figPelota = pelota;
    figPelota->SetMaterial(matPelota);
    figPelota->Translate(glm::vec3(0.0f, 30.0f, 30.0f));

    // Vehículo Tron
    motoTron = moto;
    motoTron->Rotate(180, glm::vec3(0.0f, 1.0f, 0.0f));
    motoTron->Translate(glm::vec3(0, 4.5f, 0));
}

//
// FUNCIÓN: CGScene::~CGScene()
//
// PROPÓSITO: Destruye los recursos asociados a la escena
//
CGScene::~CGScene()
{
    delete ground;
    delete figMuroDetras;
    delete figMuroDelanteIzquierdo;
    delete figMuroDelanteDerecho;
    delete figMuroIzquierdo;
    delete figMuroDerecho;
    delete figPorteria;

    delete light;
    delete matground;
    delete matMuros;
    delete matPelota;
    delete matPorteria;

    delete figPelota;
    delete motoTron;
}

//
// FUNCIÓN: CGScene::Draw()
//
// PROPÓSITO: Dibuja todos los elementos de la escena
//
void CGScene::Draw(CGShaderProgram* program, glm::mat4 proj, glm::mat4 view, glm::mat4 shadowViewMatrix)
{
    light->SetUniforms(program);

    ground->Draw(program, proj, view, shadowViewMatrix);
    figPelota->Draw(program, proj, view, shadowViewMatrix);

    figMuroDetras->Draw(program, proj, view, shadowViewMatrix);
    figMuroDelanteIzquierdo->Draw(program, proj, view, shadowViewMatrix);
    figMuroDelanteDerecho->Draw(program, proj, view, shadowViewMatrix);
    figMuroIzquierdo->Draw(program, proj, view, shadowViewMatrix);
    figMuroDerecho->Draw(program, proj, view, shadowViewMatrix);
    figPorteria->Draw(program, proj, view, shadowViewMatrix);

    motoTron->Draw(program, proj, view, shadowViewMatrix);
}

//
// FUNCIÓN: CGScene::GetLightViewMatrix()
//
// PROPÓSITO: Obtiene la matriz de posicionamiento de la luz para Shadow Mapping
//
glm::mat4 CGScene::GetLightViewMatrix()
{
    glm::vec3 Zdir = -(light->GetLightDirection());
    glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 Xdir = glm::normalize(glm::cross(Up, Zdir));
    glm::vec3 Ydir = glm::cross(Zdir, Xdir);
    glm::vec3 Zpos = 150.0f * Zdir;
    glm::vec3 Center = glm::vec3(0.0f, 0.0f, 0.0f);

    glm::mat4 view = glm::lookAt(Zpos, Center, Ydir);
    return view;
}
