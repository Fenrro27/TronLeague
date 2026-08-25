#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include "CGMaterial.h"
#include "CGShaderProgram.h"

#define VERTEX_DATA 0
#define INDEX_DATA 1
#define NORMAL_DATA 2
#define TEXTURE_DATA 3

//
// CLASE: CGFigure
//
// DESCRIPCIÓN: Clase abstracta que representa un objeto geométrico descrito mediante
//              VAO/VBOs para su renderizado mediante shaders
// 
class CGFigure {
protected:
	GLushort* indexes = nullptr; // Array de índices 
	GLfloat* vertices = nullptr; // Array de vértices
	GLfloat* normals = nullptr;  // Array de normales
	GLfloat* textures = nullptr; // Array de coordenadas de textura

	GLuint numFaces = 0;    // Número de caras
	GLuint numVertices = 0; // Número de vértices
	GLuint VBO[4] = {0, 0, 0, 0};
	GLuint VAO = 0;

	glm::mat4 location = glm::mat4(1.0f);
	CGMaterial* material = nullptr;

public:
	virtual ~CGFigure();
	void InitBuffers();
	void SetMaterial(CGMaterial* mat);
	void ResetLocation();
	void Translate(glm::vec3 t);
	void Rotate(GLfloat angle, glm::vec3 axis);
	void Draw(CGShaderProgram* program, glm::mat4 projection, glm::mat4 view, glm::mat4 shadowViewMatrix);

	glm::mat4 getLocation() { return this->location; }
	void setLocation(glm::mat4 loc) { location = loc; }
	glm::vec3 GetPosition() { return glm::vec3(location[3]); }
};
