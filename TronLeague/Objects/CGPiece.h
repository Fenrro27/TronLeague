#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "CGMaterial.h"
#include "CGShaderProgram.h"

#define VERTEX_DATA     0
#define INDEX_DATA      1
#define NORMAL_DATA     2
#define TEXTURE_DATA    3

//
// CLASE: CGPiece
//
// DESCRIPCIÓN: Clase abstracta que representa una sub-pieza descrita mediante VAO/VBOs
// 
class CGPiece {
protected:
	GLushort* indexes = nullptr;
	GLfloat* vertices = nullptr;
	GLfloat* normals = nullptr;
	GLfloat* textures = nullptr;

	int numFaces = 0;
	int numVertices = 0;

	GLuint VBO[4] = {0, 0, 0, 0};
	GLuint VAO = 0;

	glm::mat4 location = glm::mat4(1.0f);
	CGMaterial* material = nullptr;

public:
	virtual ~CGPiece();
	void InitBuffers();
	void SetMaterial(CGMaterial* m);
	void SetLocation(glm::mat4 loc);
	glm::mat4 GetLocation();
	void Translate(glm::vec3 t);
	void Rotate(GLfloat angle, glm::vec3 axis);
	void Draw(CGShaderProgram* program, glm::mat4 projection, glm::mat4 view, glm::mat4 shadowViewMatrix, glm::mat4 model);
};
