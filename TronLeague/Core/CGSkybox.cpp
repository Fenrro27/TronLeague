#include "CGSkybox.h"
#include <GL/glew.h>
#include <stb_image.h>
#include <iostream>

//
// FUNCIÓN: CGSkybox::CGSkybox()
//
// PROPÓSITO: Construye el objeto que describe la imagen de fondo
//
CGSkybox::CGSkybox()
{
	InitCubemap();
	InitCube();
}

//
// FUNCIÓN: CGSkybox::~CGSkybox()
//
// PROPÓSITO: Destruye el objeto que describe la imagen de fondo
//
CGSkybox::~CGSkybox()
{
	// Delete buffers
	glDeleteBuffers(2, VBO);
	glDeleteVertexArrays(1, &VAO);
	glDeleteTextures(1, &cubemap);
}

//
// FUNCIÓN: CGSkybox::InitCube()
//
// PROPÓSITO: Inicializa los buffers con los vértices del telón
//
void CGSkybox::InitCube()
{
	GLfloat vertices[12] = {
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f
	};

	GLushort indexes[6] = {
		0, 1, 2,
		0, 2, 3
	};

	// Create the Vertex Array Object
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Create the Vertex Buffer Objects
	glGenBuffers(2, VBO);

	// Copy data to video memory
	// Vertex data
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 12, vertices, GL_STATIC_DRAW);

	// Indexes
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * 6, indexes, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0); // Vertex position
	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

//
// FUNCIÓN: CGSkybox::InitCubemap()
//
// PROPÓSITO: Inicializa las texturas del cubo
//
void CGSkybox::InitCubemap()
{
	glActiveTexture(GL_TEXTURE0);

	glGenTextures(1, &cubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

	// Carga de texturas para cada cara
	InitTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_X, "textures/posx.jpg");
	InitTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, "textures/negx.jpg");
	InitTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, "textures/posy.jpg");
	InitTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, "textures/negy.jpg");
	InitTexture(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, "textures/posz.jpg");
	InitTexture(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, "textures/negz.jpg");

	// Configuración del cubemap
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

//
// FUNCIÓN: CGSkybox::InitTexture(GLuint target, const char* filename)
//
// PROPÓSITO: Carga una textura a partir de un fichero mediante stb_image
//
void CGSkybox::InitTexture(GLuint target, const char* filename)
{
	int nWidth = 0, nHeight = 0, channels = 0;
	stbi_set_flip_vertically_on_load(false);
	unsigned char* data = stbi_load(filename, &nWidth, &nHeight, &channels, 4);

	if (!data)
	{
		std::cerr << "Error: No se pudo cargar la textura de skybox desde: " << filename << std::endl;
		return;
	}

	glTexImage2D(target, 0, GL_RGBA8, nWidth, nHeight,
		0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);
}

//
// FUNCIÓN: CGSkybox::Draw()
//
// PROPÓSITO: Dibuja la imagen de fondo
//
void CGSkybox::Draw(CGShaderProgram* program, glm::mat4 projection, glm::mat4 view)
{
	glm::mat3 rot3 = glm::mat3(view); // Parte rotacional de la matriz View
	glm::mat4 rot4 = glm::mat4(rot3);
	glm::mat4 mvp = projection * rot4; // Transformación del Skybox a coordenadas Clip
	glm::mat4 inv = glm::inverse(mvp); // Transformación de coordenadas Clip a coordenadas de modelo del Skybox

	program->SetUniformMatrix4("Inverse", inv);
	program->SetUniformI("CubemapTex", 0);

	glDepthMask(GL_FALSE);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, NULL);
	glDepthMask(GL_TRUE);
}
