#pragma once
#include "Obxecto.h"

#include <glm/glm.hpp>

#define ACELERACION 0.004f
//#define ROZAMENTO 0
#define ROZAMENTO 0.00005f

class ObxectoMobil : public Obxecto
{
public:
	float angulo[3];
	float aceleracion;
	float rozamento;
	float limitesPosicion[6];
	float velocidade[3];
	bool xirar;

	// Para as articulacions
	ObxectoMobil(float* posicion, float* escalado, float* angulo, int tipo);
	// Para a base
	ObxectoMobil(float* posicion, float* escalado, float* angulo,
		float* limitesPosicion, float* velocidade, int tipo);

	void rotarObxecto(glm::mat4* transform, glm::mat4* transformTemp) override;
	void actualizarPosicion(float vxInicial, float vzInicial, float deltaTime);

private:
	void asignarAngulo(float* angulo);
	void inicializarCero();
	float moduloVelocidade();
};

