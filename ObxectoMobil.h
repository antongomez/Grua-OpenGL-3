#pragma once
#include "Obxecto.h"

#define ACELERACION 0.004f

class ObxectoMobil : public Obxecto
{
public:
	float angulo[3];
	float aceleracion;
	float rozamento;
	float limitesPosicion[6];
	float velocidade[3];

	ObxectoMobil(float* posicion, float* escalado, float* angulo);
	ObxectoMobil(float* posicion, float* escalado, float* angulo,
		float aceleracion, float rozamento, float* limitesPosicion,
		float* velocidade);

	void actualizarPosicion(float vxInicial, float vzInicial, float deltaTime);
private:
	void asignarAngulo(float* angulo);
	void inicializarCero();
};

