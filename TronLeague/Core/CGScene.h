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
// DESCRIPCIÓN: Construye la escena formada por los muros, terreno, portería, pelota y moto Tron
//
class CGScene {
public:
    CGScene(CGObject* motoTron, CGFigure* pelota);
    ~CGScene();
    void Draw(CGShaderProgram* program, glm::mat4 proj, glm::mat4 view, glm::mat4 shadowViewMatrix);
    glm::mat4 GetLightViewMatrix();

    CGObject* motoTron;

private:
    CGFigure* ground;

    // Muros
    CGFigure* figMuroDetras;
    CGFigure* figMuroDelanteIzquierdo;
    CGFigure* figMuroDelanteDerecho;
    CGFigure* figMuroIzquierdo;
    CGFigure* figMuroDerecho;
    CGFigure* figPorteria;

    CGFigure* figPelota;

    CGLight* light;
    CGMaterial* matground;
    CGMaterial* matMuros;
    CGMaterial* matPelota;
    CGMaterial* matPorteria;
};
