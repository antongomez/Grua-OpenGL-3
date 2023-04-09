#pragma once

#include <glm/glm.hpp>

class Obxecto
{
public:

	// Atributos
	float posicion[3]; // Determina a traslacion para posicionar ao obxecto
	float escalado[3]; // Determina o escalado para redimensionar o obxecto
	int tipo;	   // 0: cubo, 1: esfera

	// Constructor
	Obxecto(float* posicion, float* escalado, int tipo);

	// Metodos
	static void debuxaEixos(unsigned int* VAO);
	static void debuxaCadrado(unsigned int* VAO);
	static void debuxaCubo(unsigned int* VAO);
	static void debuxaEsfera(unsigned int* VAO);

	void renderizarObxecto(int transformLoc, glm::mat4* transform, glm::mat4* transformTemp, unsigned int VAO);

	// Metodos virtuais
	virtual void rotarObxecto(glm::mat4* transform, glm::mat4* transformTemp);
};

