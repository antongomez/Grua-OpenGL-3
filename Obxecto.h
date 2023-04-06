#pragma once
class Obxecto
{
public:

	// Atributos
	float posicion[3]; // Determina a traslacion para posicionar ao obxecto
	float escalado[3]; // Determina o escalado para redimensionar o obxecto

	// Constructor
	Obxecto(float* posicion, float* escalado);

	// Metodos
	void debuxaCubo(unsigned int* VAO);
	void debuxaEsfera(unsigned int* VAO);
};

