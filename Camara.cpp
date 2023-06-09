#include "Camara.h"

#include <math.h>

#include <glad.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Camara::Camara(float radio, float alpha, float beta) {
	this->shaderProgram = 0;
	this->radio = radio;
	this->alpha = alpha;
	this->beta = beta;
	this->width = SCR_WIDTH;
	this->height = SCR_HEIGHT;
}




glm::vec3 Camara::obtenerPos1Pers(float x, float y, float z, float angulo)
{
	float d2 = 1.0f, alturaOllos = 0.2, posicionSilla = 0.2;
	return glm::vec3(x + posicionSilla * sin(angulo), y + 0.15, z + posicionSilla * cos(angulo));
}


void Camara::vistaPrimeiraPersoa(float x, float y, float z, float angulo)
{
	float d2 = 1.0f;

	// Matriz de vista
	glm::mat4 view;
	view = glm::mat4();
	view = glm::lookAt(
		obtenerPos1Pers(x, y, z, angulo),
		glm::vec3(x + d2 * sin(angulo), y + 0.15, z + d2 * cos(angulo)),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);
	unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
}


glm::vec3 Camara::obtenerPosXeral()
{
	return glm::vec3(radio * sin(alpha) * cos(beta), radio * sin(beta), radio * cos(alpha) * cos(beta));
}

void Camara::vistaXeral(float x, float y, float z)
{

	// Matriz de vista
	glm::mat4 view;
	view = glm::mat4();
	view = glm::lookAt(
		obtenerPosXeral(),
		glm::vec3(x, y, z),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);
	unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
}

glm::vec3 Camara::obtenerPos3Pers(float x, float y, float z, float angulo)
{
	float d1 = 0.6f, d2 = 3.0f;
	return glm::vec3(x - d1 * sin(angulo), y + 0.4, z - d1 * cos(angulo));
}

void Camara::vistaTerceiraPersoa(float x, float y, float z, float angulo)
{
	float d1 = 0.6f, d2 = 3.0f;
	// Matriz de vista
	glm::mat4 view;
	view = glm::mat4();
	view = glm::lookAt(
		obtenerPos3Pers(x, y, z, angulo),
		glm::vec3(x + d2 * sin(angulo), y, z + d2 * cos(angulo)),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);
	unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
}

void Camara::actualizarMatrizProxeccion() {

	glm::mat4 projection;
	projection = glm::mat4();
	projection = glm::perspective(
		glm::radians(60.0f),
		width / height,
		0.1f, 10.0f
	);
	unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}