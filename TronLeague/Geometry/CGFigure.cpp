#include "CGFigure.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//
// FUNCIÓN: CGFigure::~CGFigure()
//
// PROPÓSITO: Destructor de la figura y liberación de buffers GPU
//
CGFigure::~CGFigure()
{
	if (vertices != nullptr) { delete[] vertices; vertices = nullptr; }
	if (indexes != nullptr) { delete[] indexes; indexes = nullptr; }
	if (normals != nullptr) { delete[] normals; normals = nullptr; }
	if (textures != nullptr) { delete[] textures; textures = nullptr; }

	if (VBO[0] != 0) glDeleteBuffers(4, VBO);
	if (VAO != 0) glDeleteVertexArrays(1, &VAO);
}

//
// FUNCIÓN: CGFigure::InitBuffers()
//
// PROPÓSITO: Crea el VAO y los VBOs y transfiere los datos a la memoria de vídeo
//
void CGFigure::InitBuffers()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(4, VBO);

	// Vértices
	int buffsize = sizeof(GLfloat) * numVertices * 3;
	glBindBuffer(GL_ARRAY_BUFFER, VBO[VERTEX_DATA]);
	glBufferData(GL_ARRAY_BUFFER, buffsize, vertices, GL_STATIC_DRAW);

	// Normales
	glBindBuffer(GL_ARRAY_BUFFER, VBO[NORMAL_DATA]);
	glBufferData(GL_ARRAY_BUFFER, buffsize, normals, GL_STATIC_DRAW);

	// Coordenadas de textura
	buffsize = sizeof(GLfloat) * numVertices * 2;
	glBindBuffer(GL_ARRAY_BUFFER, VBO[TEXTURE_DATA]);
	glBufferData(GL_ARRAY_BUFFER, buffsize, textures, GL_STATIC_DRAW);

	// Índices
	buffsize = sizeof(GLushort) * numFaces * 3;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[INDEX_DATA]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, buffsize, indexes, GL_STATIC_DRAW);

	delete[] vertices; vertices = nullptr;
	delete[] indexes; indexes = nullptr;
	delete[] normals; normals = nullptr;
	delete[] textures; textures = nullptr;

	glEnableVertexAttribArray(0); // Posición
	glBindBuffer(GL_ARRAY_BUFFER, VBO[VERTEX_DATA]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(1); // Normales
	glBindBuffer(GL_ARRAY_BUFFER, VBO[NORMAL_DATA]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(2); // Texturas
	glBindBuffer(GL_ARRAY_BUFFER, VBO[TEXTURE_DATA]);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

	location = glm::mat4(1.0f);
}

void CGFigure::SetMaterial(CGMaterial* mat)
{
	material = mat;
}

void CGFigure::ResetLocation()
{
	location = glm::mat4(1.0f);
}

void CGFigure::Translate(glm::vec3 t)
{
	location = glm::translate(location, t);
}

void CGFigure::Rotate(GLfloat angle, glm::vec3 axis)
{
	location = glm::rotate(location, glm::radians(angle), axis);
}

void CGFigure::Draw(CGShaderProgram* program, glm::mat4 projection, glm::mat4 view, glm::mat4 shadowViewMatrix)
{
	glm::mat4 mvp = projection * view * location;
	program->SetUniformMatrix4("MVP", mvp);
	program->SetUniformMatrix4("ViewMatrix", view);
	program->SetUniformMatrix4("ModelViewMatrix", view * location);
	program->SetUniformMatrix4("ShadowMatrix", shadowViewMatrix * location);
	if (material) material->SetUniforms(program);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, numFaces * 3, GL_UNSIGNED_SHORT, NULL);
}
