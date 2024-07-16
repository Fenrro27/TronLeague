
#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "CGShaderProgram.h"
#include "CGLight.h"
#include "CGMaterial.h"
#include "CGFigure.h"
#include "CGObject.h"

//
// CLASE: CGScene
//
// DESCRIPCIÓN: Construye una escena formada por varias figuras geométricas
//
class CGScene {
public:
    CGScene(CGObject* motoTron, CGFigure* Pelota);
    ~CGScene();
    void Draw(CGShaderProgram* program, glm::mat4 proj, glm::mat4 view, glm::mat4 shadowViewMatrix);
    glm::mat4 GetLightViewMatrix();

    CGObject* motoTron;

private:
    CGFigure* ground;
    //Muros
    CGFigure* figMuroDetras;
    CGFigure* figMuroDelanteIzquierdo;
    CGFigure* figMuroDelanteDerecho;
    CGFigure* figMuroIzquierdo;
    CGFigure* figMuroDerecho;
    CGFigure* figPorteria;

    CGFigure* figPelota;



    CGLight* light;
    CGMaterial* matground; // material del suelo
    CGMaterial* matMuros;
    CGMaterial* mat1;
    CGMaterial* mat2;
    CGMaterial* mat3;
    CGMaterial* matPelota; // Material de la pelota
    CGMaterial* mat5;
    CGMaterial* matPorteria;
};