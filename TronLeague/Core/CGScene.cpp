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
// PROPÓSITO: Construye el estadio, iluminación, materiales y objetos
//
CGScene::CGScene(CGObject* moto, CGFigure* pelota)
{
    glm::vec3 Ldir = glm::vec3(0.8f, -1.0f, -0.6f);
    Ldir = glm::normalize(Ldir);
    light = new CGLight();
    light->SetLightDirection(Ldir);
    light->SetAmbientLight(glm::vec3(0.35f, 0.35f, 0.38f));
    light->SetDifusseLight(glm::vec3(0.90f, 0.90f, 0.90f));
    light->SetSpecularLight(glm::vec3(1.0f, 1.0f, 1.0f));

    // Material y suelo del estadio
    matground = new CGMaterial();
    matground->SetAmbientReflect(1.0f, 1.0f, 1.0f);
    matground->SetDifusseReflect(1.0f, 1.0f, 1.0f);
    matground->SetSpecularReflect(0.6f, 0.6f, 0.6f);
    matground->SetShininess(32.0f);
    matground->InitTexture("textures/Stylized_Stone_Floor_005_basecolor.jpg");

    float groundWidth = 110.0f;
    float groundLength = 350.0f;
    float wallHeight = 16.0f;
    float wallThickness = 0.5f;
    float gateWidth = 46.0f;

    ground = new CGGround(groundWidth, groundLength + 20.0f);
    ground->SetMaterial(matground);

    // Muros perimetrales
    matMuros = new CGMaterial();
    matMuros->SetAmbientReflect(1.0f, 1.0f, 1.0f);
    matMuros->SetDifusseReflect(1.0f, 1.0f, 1.0f);
    matMuros->SetSpecularReflect(0.6f, 0.6f, 0.6f);
    matMuros->SetShininess(32.0f);
    matMuros->InitTexture("textures/Tiles_048_basecolor.jpg");

    // Muro trasero (Z = -groundLength)
    figMuroDetras = new CGCube(groundWidth, wallHeight, wallThickness, 1, 8);
    figMuroDetras->SetMaterial(matMuros);
    figMuroDetras->Translate(glm::vec3(0.0f, wallHeight / 2.0f, -groundLength));
    figMuroDetras->Rotate(180.0f, glm::vec3(0.0f, 1.0f, 0.0f));

    // Muros frontales junto a la portería (Z = +groundLength)
    float sideWallW = (groundWidth / 2.0f) - (gateWidth / 2.0f);
    figMuroDelanteIzquierdo = new CGCube(sideWallW, wallHeight, wallThickness, 1, 2);
    figMuroDelanteIzquierdo->SetMaterial(matMuros);
    figMuroDelanteIzquierdo->Translate(glm::vec3((groundWidth / 2.0f) + (gateWidth / 2.0f), wallHeight / 2.0f, groundLength));

    figMuroDelanteDerecho = new CGCube(sideWallW, wallHeight, wallThickness, 1, 2);
    figMuroDelanteDerecho->SetMaterial(matMuros);
    figMuroDelanteDerecho->Translate(glm::vec3(-(groundWidth / 2.0f) - (gateWidth / 2.0f), wallHeight / 2.0f, groundLength));

    // Muro lateral izquierdo (X = +groundWidth)
    figMuroIzquierdo = new CGCube(groundLength, wallHeight, wallThickness, 1, 16);
    figMuroIzquierdo->SetMaterial(matMuros);
    figMuroIzquierdo->Translate(glm::vec3(groundWidth, wallHeight / 2.0f, 0.0f));
    figMuroIzquierdo->Rotate(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));

    // Muro lateral derecho (X = -groundWidth)
    figMuroDerecho = new CGCube(groundLength, wallHeight, wallThickness, 1, 16);
    figMuroDerecho->SetMaterial(matMuros);
    figMuroDerecho->Translate(glm::vec3(-groundWidth, wallHeight / 2.0f, 0.0f));
    figMuroDerecho->Rotate(270.0f, glm::vec3(0.0f, 1.0f, 0.0f));

    // Portería
    matPorteria = new CGMaterial();
    matPorteria->SetAmbientReflect(1.0f, 1.0f, 1.0f);
    matPorteria->SetDifusseReflect(1.0f, 1.0f, 1.0f);
    matPorteria->SetSpecularReflect(0.9f, 0.9f, 0.9f);
    matPorteria->SetShininess(32.0f);
    matPorteria->InitTexture("textures/goal.jpg");

    figPorteria = new Porteria(gateWidth, wallHeight + 2.0f, 16.0f);
    figPorteria->SetMaterial(matPorteria);
    figPorteria->Translate(glm::vec3(0.0f, (wallHeight / 2.0f) + 1.0f, groundLength + 8.0f));
    figPorteria->Rotate(180.0f, glm::vec3(0.0f, 1.0f, 0.0f));

    // Pelota
    matPelota = new CGMaterial();
    matPelota->SetAmbientReflect(1.0f, 1.0f, 1.0f);
    matPelota->SetDifusseReflect(1.0f, 1.0f, 1.0f);
    matPelota->SetSpecularReflect(0.9f, 0.9f, 0.9f);
    matPelota->SetShininess(32.0f);
    matPelota->InitTexture("textures/ball4.png");

    figPelota = pelota;
    figPelota->SetMaterial(matPelota);

    motoTron = moto;
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
// PROPÓSITO: Obtiene la matriz de vista de la luz direccional para Shadow Mapping
//
glm::mat4 CGScene::GetLightViewMatrix()
{
    glm::vec3 Zdir = -(light->GetLightDirection());
    glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 Xdir = glm::normalize(glm::cross(Up, Zdir));
    glm::vec3 Ydir = glm::cross(Zdir, Xdir);
    glm::vec3 Zpos = 200.0f * Zdir;
    glm::vec3 Center = glm::vec3(0.0f, 0.0f, 0.0f);

    return glm::lookAt(Zpos, Center, Ydir);
}
