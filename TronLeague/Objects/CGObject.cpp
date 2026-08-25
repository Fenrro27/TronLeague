#include "CGObject.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void CGObject::ResetLocation()
{
	model = glm::mat4(1.0f);
}

void CGObject::SetLocation(glm::mat4 loc)
{
	model = loc;
}

glm::mat4 CGObject::GetLocation()
{
	return model;
}

void CGObject::Translate(glm::vec3 t)
{
	model = glm::translate(model, t);
}

void CGObject::Rotate(GLfloat angle, glm::vec3 axis)
{
	model = glm::rotate(model, glm::radians(angle), axis);
}

void CGObject::Draw(CGShaderProgram* program, glm::mat4 projection, glm::mat4 view, glm::mat4 shadowViewMatrix)
{
	int num = GetNumPieces();
	for (int i = 0; i < num; i++)
	{
		GetPiece(i)->Draw(program, projection, view, shadowViewMatrix, model);
	}
}

glm::vec3 CGObject::GetPosition()
{
	return glm::vec3(model[3]);
}

glm::vec3 CGObject::GetDirection()
{
	return glm::normalize(glm::vec3(model[2]));
}
