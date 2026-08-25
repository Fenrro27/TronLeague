#include "CGPiece.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "CGMaterial.h"

//
// FUNCIÓN: CGPiece::~CGPiece()
//
// PROPÓSITO: Destructor de la pieza
//
CGPiece::~CGPiece()
{
	if (vertices != nullptr) { delete[] vertices; vertices = nullptr; }
	if (indexes != nullptr) { delete[] indexes; indexes = nullptr; }
	if (normals != nullptr) { delete[] normals; normals = nullptr; }
	if (textures != nullptr) { delete[] textures; textures = nullptr; }

	if (VBO[0] != 0) glDeleteBuffers(4, VBO);
	if (VAO != 0) glDeleteVertexArrays(1, &VAO);
}

//
// FUNCIÓN: CGPiece::InitBuffers()
//
// PROPÓSITO: Crea el VAO y los VBOs y almacena los datos en memoria GPU
//
void CGPiece::InitBuffers()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(4, VBO);

	// Vértices
	glBindBuffer(GL_ARRAY_BUFFER, VBO[VERTEX_DATA]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * numVertices * 3, vertices, GL_STATIC_DRAW);

	// Normales
	glBindBuffer(GL_ARRAY_BUFFER, VBO[NORMAL_DATA]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * numVertices * 3, normals, GL_STATIC_DRAW);

	// Texturas
	glBindBuffer(GL_ARRAY_BUFFER, VBO[TEXTURE_DATA]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * numVertices * 2, textures, GL_STATIC_DRAW);

	// Índices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[INDEX_DATA]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * numFaces * 3, indexes, GL_STATIC_DRAW);

	delete[] vertices; vertices = nullptr;
	delete[] indexes; indexes = nullptr;
	delete[] normals; normals = nullptr;
	delete[] textures; textures = nullptr;

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[VERTEX_DATA]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[NORMAL_DATA]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO[TEXTURE_DATA]);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

	location = glm::mat4(1.0f);
}

void CGPiece::SetMaterial(CGMaterial* m)
{
	material = m;
}

void CGPiece::SetLocation(glm::mat4 loc)
{
	location = loc;
}

glm::mat4 CGPiece::GetLocation()
{
	return location;
}

void CGPiece::Translate(glm::vec3 t)
{
	location = glm::translate(location, t);
}

void CGPiece::Rotate(GLfloat angle, glm::vec3 axis)
{
	location = glm::rotate(location, glm::radians(angle), axis);
}

void CGPiece::Draw(CGShaderProgram* program, glm::mat4 projection, glm::mat4 view, glm::mat4 shadowViewMatrix, glm::mat4 model)
{
	glm::mat4 mvp = projection * view * model * location;
	program->SetUniformMatrix4("MVP", mvp);
	program->SetUniformMatrix4("ViewMatrix", view);
	program->SetUniformMatrix4("ModelViewMatrix", view * model * location);
	program->SetUniformMatrix4("ShadowMatrix", shadowViewMatrix * model * location);

	if (material) material->SetUniforms(program);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, numFaces * 3, GL_UNSIGNED_SHORT, NULL);
}
