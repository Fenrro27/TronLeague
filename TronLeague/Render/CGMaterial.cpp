#include "CGMaterial.h"
#include <GL/glew.h>
#include <stb_image.h>
#include <iostream>

//
// FUNCIÓN: CGMaterial::CGMaterial()
//
// PROPÓSITO: Construye un material con los valores por defecto
//
CGMaterial::CGMaterial()
{
	Ka = glm::vec3(1.0f, 1.0f, 1.0f);
	Kd = glm::vec3(1.0f, 1.0f, 1.0f);
	Ks = glm::vec3(0.8f, 0.8f, 0.8f);
	Shininess = 16.0f;
	textureId = 0;
}

//
// FUNCIÓN: CGMaterial::~CGMaterial()
//
// PROPÓSITO: Libera recursos de la textura asociada
//
CGMaterial::~CGMaterial()
{
	if (textureId != 0)
	{
		glDeleteTextures(1, &textureId);
		textureId = 0;
	}
}

//
// FUNCIÓN: CGMaterial::SetAmbientReflect(GLfloat r, GLfloat g, GLfloat b)
//
// PROPÓSITO: Asigna la reflectividad ambiental (color ante la luz ambiental)
//
void CGMaterial::SetAmbientReflect(GLfloat r, GLfloat g, GLfloat b)
{
	Ka = glm::vec3(r, g, b);
}

//
// FUNCIÓN: CGMaterial::SetDifusseReflect(GLfloat r, GLfloat g, GLfloat b)
//
// PROPÓSITO: Asigna la reflectividad difusa (color ante la luz difusa)
//
void CGMaterial::SetDifusseReflect(GLfloat r, GLfloat g, GLfloat b)
{
	Kd = glm::vec3(r, g, b);
}

//
// FUNCIÓN: CGMaterial::SetSpecularReflect(GLfloat r, GLfloat g, GLfloat b)
//
// PROPÓSITO: Asigna la reflectividad especular (color ante la luz especular)
//
void CGMaterial::SetSpecularReflect(GLfloat r, GLfloat g, GLfloat b)
{
	Ks = glm::vec3(r, g, b);
}

//
// FUNCIÓN: CGMaterial::SetShininess(GLfloat f)
//
// PROPÓSITO: Asigna el factor de brillo (comportamiento ante la luz especular)
//
void CGMaterial::SetShininess(GLfloat f)
{
	Shininess = f;
}

//
// FUNCIÓN: CGMaterial::SetUniforms(CGShaderProgram* program)
//
// PROPÓSITO: Configura las propiedades de material en el programa gráfico
//
void CGMaterial::SetUniforms(CGShaderProgram* program)
{
	program->SetUniformVec3("Material.Ka", Ka);
	program->SetUniformVec3("Material.Kd", Kd);
	program->SetUniformVec3("Material.Ks", Ks);
	program->SetUniformF("Material.Shininess", Shininess);
	program->SetUniformI("UseTexture", textureId != 0 ? 1 : 0);
	program->SetUniformI("BaseTex", 0);

	if (textureId != 0)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureId);
	}
}

//
// FUNCIÓN: CGMaterial::SetTexture(GLuint id)
//
// PROPÓSITO: Asigna el identificador de la textura básica
//
void CGMaterial::SetTexture(GLuint id)
{
	textureId = id;
}

//
// FUNCIÓN: CGMaterial::GetTexture()
//
// PROPÓSITO: Obtiene el identificador de la textura básica
//
GLuint CGMaterial::GetTexture()
{
	return textureId;
}

//
// FUNCIÓN: CGMaterial::InitTexture(const char* filename)
//
// PROPÓSITO: Carga una textura a partir de un fichero mediante stb_image
//
void CGMaterial::InitTexture(const char* filename)
{
	int nWidth = 0, nHeight = 0, channels = 0;
	stbi_set_flip_vertically_on_load(false);
	unsigned char* data = stbi_load(filename, &nWidth, &nHeight, &channels, 4);

	if (!data)
	{
		std::cerr << "Error: No se pudo cargar la textura desde: " << filename << std::endl;
		return;
	}

	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, nWidth, nHeight,
		0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	stbi_image_free(data);
}
