#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "CGShaderProgram.h"
#include "CGPiece.h"

//
// CLASE: CGObject
//
// DESCRIPCIÓN: Clase abstracta que representa un objeto compuesto formado por piezas
// 
class CGObject
{
protected:
	glm::mat4 model = glm::mat4(1.0f);

public:
	virtual ~CGObject() = default;
	void ResetLocation();
	void Translate(glm::vec3 t);
	void Rotate(GLfloat angle, glm::vec3 axis);
	void SetLocation(glm::mat4 loc);
	glm::mat4 GetLocation();
	void Draw(CGShaderProgram* program, glm::mat4 projection, glm::mat4 view, glm::mat4 shadowViewMatrix);

	virtual int GetNumPieces() = 0;
	virtual CGPiece* GetPiece(int i) = 0;

	glm::vec3 GetPosition();
	glm::vec3 GetDirection();
};
