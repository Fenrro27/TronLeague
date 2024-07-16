#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#define NO_SHADER 0xffff

//
// CLASE: CGShaderProgram
//
// DESCRIPCIÓN: Clase que desarrolla un programa GLSL
// 
class CGShaderProgram {
private:
	GLuint program;
	GLuint vertexShader;
	GLuint fragmentShader;
	GLuint geometryShader;
	GLuint tessControlShader;
	GLuint tessEvaluationShader;
	GLboolean linked;

	char* GetShaderCodeFromResource(int idr);
	char* GetShaderCodeFromFile(const char* filename);
	GLuint CreateShader(int mode, int idr);
	GLuint CreateShader(int mode, const char* filename);

public:
	CGShaderProgram(int vs, int fs, int gs, int tcs, int tes);
	CGShaderProgram(const char* vs, const char* fs, const char* gs, const char* tcs, const char* tes);
	~CGShaderProgram();
	GLboolean IsLinked();
	GLvoid Use();
	GLvoid SetUniformF(const char* name, GLfloat f);
	GLvoid SetUniformVec4(const char* name, glm::vec4 m);
	GLvoid SetUniformVec3(const char* name, glm::vec3 m);
	GLvoid SetUniformMatrix4(const char* name, glm::mat4 m);
	GLvoid SetUniformI(const char* name, GLint i);
	GLvoid SetUniformSubroutine(int mode, const char* name);
};
