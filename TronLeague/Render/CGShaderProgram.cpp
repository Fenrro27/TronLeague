#include "CGShaderProgram.h"
#include <GL/glew.h>
#include <iostream>
#include <cstdlib>
#include <cstring>

//
// FUNCIÓN: CGShaderProgram::CGShaderProgram(const char* vs, const char* fs, const char* gs, const char* tcs, const char* tes)
//
// PROPÓSITO: Crea un programa gráfico cargando y compilando los shaders que lo forman desde archivos
//
CGShaderProgram::CGShaderProgram(const char* vs, const char* fs, const char* gs, const char* tcs, const char* tes)
{
	vertexShader = NO_SHADER;
	fragmentShader = NO_SHADER;
	geometryShader = NO_SHADER;
	tessControlShader = NO_SHADER;
	tessEvaluationShader = NO_SHADER;
	linked = GL_FALSE;

	// Crea y compila los shaders
	if (vs != nullptr) vertexShader = CreateShader(GL_VERTEX_SHADER, vs);
	if (fs != nullptr) fragmentShader = CreateShader(GL_FRAGMENT_SHADER, fs);
	if (gs != nullptr) geometryShader = CreateShader(GL_GEOMETRY_SHADER, gs);
	if (tcs != nullptr) tessControlShader = CreateShader(GL_TESS_CONTROL_SHADER, tcs);
	if (tes != nullptr) tessEvaluationShader = CreateShader(GL_TESS_EVALUATION_SHADER, tes);

	// Crea el programa y adjunta los shaders
	program = glCreateProgram();
	if (vertexShader != NO_SHADER) glAttachShader(program, vertexShader);
	if (fragmentShader != NO_SHADER) glAttachShader(program, fragmentShader);
	if (geometryShader != NO_SHADER) glAttachShader(program, geometryShader);
	if (tessControlShader != NO_SHADER) glAttachShader(program, tessControlShader);
	if (tessEvaluationShader != NO_SHADER) glAttachShader(program, tessEvaluationShader);
	glLinkProgram(program);

	GLint status;
	glGetProgramiv(program, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		linked = GL_FALSE;
		GLint logLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
		if (logLength > 0)
		{
			char* logInfo = (char*)malloc(sizeof(char) * logLength);
			GLsizei written;
			glGetProgramInfoLog(program, logLength, &written, logInfo);
			std::cerr << "Error de enlace en ShaderProgram: " << logInfo << std::endl;
			free(logInfo);
		}
		return;
	}

	linked = GL_TRUE;
}

//
// FUNCIÓN: CGShaderProgram::~CGShaderProgram()
//
// PROPÓSITO: Destruye el programa gráfico y libera shaders
//
CGShaderProgram::~CGShaderProgram()
{
	if (vertexShader != NO_SHADER) glDeleteShader(vertexShader);
	if (fragmentShader != NO_SHADER) glDeleteShader(fragmentShader);
	if (geometryShader != NO_SHADER) glDeleteShader(geometryShader);
	if (tessControlShader != NO_SHADER) glDeleteShader(tessControlShader);
	if (tessEvaluationShader != NO_SHADER) glDeleteShader(tessEvaluationShader);

	glDeleteProgram(program);
}

//
// FUNCIÓN: CGShaderProgram::CreateShader(int mode, const char* filename)
//
// PROPÓSITO: Crea un shader de un cierto tipo a partir de un fichero
//
GLuint CGShaderProgram::CreateShader(int mode, const char* filename)
{
	GLint status;
	GLuint shader = glCreateShader(mode);
	char* code = GetShaderCodeFromFile(filename);
	if (!code)
	{
		std::cerr << "No se pudo leer el archivo de shader: " << filename << std::endl;
		glDeleteShader(shader);
		return NO_SHADER;
	}

	glShaderSource(shader, 1, &code, NULL);
	glCompileShader(shader);
	free(code);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		GLint logLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
		if (logLength > 0)
		{
			char* logInfo = (char*)malloc(sizeof(char) * logLength);
			GLsizei written;
			glGetShaderInfoLog(shader, logLength, &written, logInfo);
			std::cerr << "Error de compilación en shader (" << filename << "): " << logInfo << std::endl;
			free(logInfo);
		}
		glDeleteShader(shader);
		return NO_SHADER;
	}
	return shader;
}

//
// FUNCIÓN: CGShaderProgram::GetShaderCodeFromFile(const char* filename)
//
// PROPÓSITO: Obtiene el contenido de un fichero de texto de shader
//
char* CGShaderProgram::GetShaderCodeFromFile(const char* filename)
{
	FILE* file = fopen(filename, "rb");
	if (!file)
	{
		return nullptr;
	}
	fseek(file, 0L, SEEK_END);
	long size = ftell(file);
	fseek(file, 0L, SEEK_SET);

	char* code = (char*)malloc(sizeof(char) * (size + 1));
	if (!code)
	{
		fclose(file);
		return nullptr;
	}

	size_t read = fread(code, sizeof(char), size, file);
	code[read] = '\0';
	fclose(file);
	return code;
}

//
// FUNCIÓN: CGShaderProgram::IsLinked()
//
// PROPÓSITO: Verifica si el programa se ha enlazado de forma correcta
//
GLboolean CGShaderProgram::IsLinked()
{
	return linked;
}

//
// FUNCIÓN: CGShaderProgram::Use()
//
// PROPÓSITO: Activa el programa en el pipeline gráfico
//
GLvoid CGShaderProgram::Use()
{
	glUseProgram(program);
}

//
// FUNCIÓN: CGShaderProgram::SetUniformF(const char* name, GLfloat f)
//
// PROPÓSITO: Asigna el valor de una variable uniforme de tipo float
//
GLvoid CGShaderProgram::SetUniformF(const char* name, GLfloat f)
{
	GLint location = glGetUniformLocation(program, name);
	if (location >= 0) glUniform1f(location, f);
}

//
// FUNCIÓN: CGShaderProgram::SetUniformMatrix4(const char* name, glm::mat4 m)
//
// PROPÓSITO: Asigna el valor de una variable uniforme de tipo mat4 (matriz 4x4)
//
GLvoid CGShaderProgram::SetUniformMatrix4(const char* name, glm::mat4 m)
{
	GLint location = glGetUniformLocation(program, name);
	if (location >= 0) glUniformMatrix4fv(location, 1, GL_FALSE, &m[0][0]);
}

//
// FUNCIÓN: CGShaderProgram::SetUniformVec4(const char* name, glm::vec4 v)
//
// PROPÓSITO: Asigna el valor de una variable uniforme de tipo vec4
//
GLvoid CGShaderProgram::SetUniformVec4(const char* name, glm::vec4 v)
{
	GLint location = glGetUniformLocation(program, name);
	if (location >= 0) glUniform4fv(location, 1, &v[0]);
}

//
// FUNCIÓN: CGShaderProgram::SetUniformVec3(const char* name, glm::vec3 v)
//
// PROPÓSITO: Asigna el valor de una variable uniforme de tipo vec3
//
GLvoid CGShaderProgram::SetUniformVec3(const char* name, glm::vec3 v)
{
	GLint location = glGetUniformLocation(program, name);
	if (location >= 0) glUniform3fv(location, 1, &v[0]);
}

//
// FUNCIÓN: CGShaderProgram::SetUniformI(const char* name, GLint i)
//
// PROPÓSITO: Asigna el valor de una variable uniforme de tipo entero
//
GLvoid CGShaderProgram::SetUniformI(const char* name, GLint i)
{
	GLint location = glGetUniformLocation(program, name);
	if (location >= 0) glUniform1i(location, i);
}

//
// FUNCIÓN: CGShaderProgram::SetUniformSubroutine(int mode, const char* name)
//
// PROPÓSITO: Asigna una subrutina a uno de los shaders
//
GLvoid CGShaderProgram::SetUniformSubroutine(int mode, const char* name)
{
	GLuint location = glGetSubroutineIndex(program, mode, name);
	if (location != GL_INVALID_INDEX) glUniformSubroutinesuiv(mode, 1, &location);
}
