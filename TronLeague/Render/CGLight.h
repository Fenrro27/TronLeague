#pragma once

#include <glm/glm.hpp>
#include "CGShaderProgram.h"

//
// CLASE: CGLight
//
// DESCRIPCIÓN: Describe las propiedades de la luz en una escena
//
class CGLight {

private:
	glm::vec3 Ldir; // Dirección de la luz
	glm::vec3 La;   // Intensidad ambiental
	glm::vec3 Ld;   // Intensidad difusa
	glm::vec3 Ls;   // Intensidad especular

public:
	CGLight();
	glm::vec3 GetLightDirection();
	void SetLightDirection(glm::vec3 d);
	void SetAmbientLight(glm::vec3 a);
	void SetDifusseLight(glm::vec3 d);
	void SetSpecularLight(glm::vec3 s);
	void SetUniforms(CGShaderProgram* program);
};
