#include "Obxecto.h"
#include "ObxectoMobil.h"

#include "math.h"


ObxectoMobil::ObxectoMobil(float* posicion, float* escalado, float* angulo) : Obxecto(posicion, escalado) {
	asignarAngulo(angulo);
	inicializarCero();
}

ObxectoMobil::ObxectoMobil(float* posicion, float* escalado, float* angulo,
	float aceleracion, float rozamento, float* limitesPosicion,
	float* velocidade) : Obxecto(posicion, escalado) {

	asignarAngulo(angulo);
	this->aceleracion = aceleracion;
	this->rozamento = rozamento;

	for (int i = 0; i < 3; i++) {
		this->velocidade[i] = velocidade[i];
	}

	for (int i = 0; i < 6; i++) {
		this->limitesPosicion[i] = limitesPosicion[i];
	}
}

void ObxectoMobil::asignarAngulo(float* angulo) {
	for (int i = 0; i < 3; i++) {
		this->angulo[i] = angulo[i];
	}
}

void ObxectoMobil::inicializarCero() {
	aceleracion = 0;
	rozamento = 0;

	for (int i = 0; i < 3; i++) {
		this->velocidade[i] = 0;
	}

	for (int i = 0; i < 6; i++) {
		this->limitesPosicion[i] = 0;
	}
}

void ObxectoMobil::actualizarPosicion(float vxInicial, float vzInicial, float deltaTime) {
	float tempvx, tempvz;

	// No caso no que se esta pisando o acelerador
	if (aceleracion > 0) {
		// Calcular a velocidade da grua en función da aceleración 
		// e do tempo que se leva pisando o acelerador
		tempvx = vxInicial + sin(angulo[1]) * aceleracion * deltaTime;
		tempvz = vzInicial + cos(angulo[1]) * aceleracion * deltaTime;
	}
	else {
		// No caso no que deixamos de acelerar a 
		// velocidade permanece constante se obviamos 
		// a forza de rozamento
		tempvx = velocidade[0];
		tempvz = velocidade[2];
	}

	// Limitamos o valor da velocidade para que non 
	// cambie de signo por culpa da forza de rozamento
	if (tempvx >= 0) {
		velocidade[0] = fmax(tempvx - rozamento, 0);
	}
	else {
		velocidade[0] = fmin(tempvx + rozamento, 0);
	}
	if (tempvz >= 0) {
		velocidade[2] = fmax(tempvz - rozamento, 0);
	}
	else {
		velocidade[2] = fmin(tempvz + rozamento, 0);
	}

	// Actualizamos a posicion da grua tendo en conta a velocidade
	posicion[0] = fmin(posicion[0] + velocidade[0], limitesPosicion[1]);
	posicion[0] = fmax(posicion[0] + velocidade[0], limitesPosicion[0]);
	posicion[2] = fmin(posicion[2] + velocidade[2], limitesPosicion[5]);
	posicion[2] = fmax(posicion[2] + velocidade[2], limitesPosicion[4]);
}


