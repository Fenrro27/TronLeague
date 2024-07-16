#pragma once

#include <GL/glew.h>
#include "CGFigure.h"

//
// CLASE: CGCube
//
// DESCRIPCIÓN: Representa un cubo de lado 2·s. 
// 
class CGCube : public CGFigure {
public:
	CGCube(GLfloat width, GLfloat height, GLfloat depth, GLfloat textureRepeatX = 1, GLfloat textureRepeatY = 20);

};

