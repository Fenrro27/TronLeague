#pragma once

#include <GL/glew.h>
#include "CGFigure.h"

//
// CLASE: Porteria
//
// DESCRIPCIÓN: Representa la portería con postes y red
// 
class Porteria : public CGFigure {
public:
	Porteria(GLfloat width, GLfloat height, GLfloat depth);
};
