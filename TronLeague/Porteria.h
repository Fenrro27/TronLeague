#pragma once

#include <GL/glew.h>
#include "CGFigure.h"

//
// CLASE: CGCube
//
// DESCRIPCIÓN: Representa un cubo de lado 2·s. 
// 
class Porteria : public CGFigure {
public:
	Porteria(GLfloat width, GLfloat height, GLfloat depth);

};