#include "Obxecto.h"
#include "ObxectoMobil.h"

#include <glm/gtc/matrix_transform.hpp>

#include "math.h"


ObxectoMobil::ObxectoMobil(float* posicion, float* escalado, float* angulo, int tipo) : Obxecto(posicion, escalado, tipo) {
	asignarAngulo(angulo);
	inicializarCero();
	this->xirar = false;
}

ObxectoMobil::ObxectoMobil(float* posicion, float* escalado, float* angulo,
	float* limitesPosicion, float* velocidade, int tipo) : Obxecto(posicion, escalado, tipo) {

	asignarAngulo(angulo);
	this->aceleracion = 0;
	this->rozamento = ROZAMENTO;

	for (int i = 0; i < 3; i++) {
		this->velocidade[i] = velocidade[i];
	}

	for (int i = 0; i < 6; i++) {
		this->limitesPosicion[i] = limitesPosicion[i];
	}

	this->xirar = true;
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

void ObxectoMobil::rotarObxecto(glm::mat4* transform, glm::mat4* transformTemp) {
	*transform = glm::rotate(*transform, this->angulo[0], glm::vec3(1.0, 0.0, 0.0));
	*transform = glm::rotate(*transform, this->angulo[1], glm::vec3(0.0, 1.0, 0.0));
	*transform = glm::rotate(*transform, this->angulo[2], glm::vec3(0.0, 0.0, 1.0));
}

float ObxectoMobil::moduloVelocidade() {
	return sqrt(velocidade[2] * velocidade[2] + velocidade[0] * velocidade[0]);
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
		/* No caso no que deixamos de acelerar a
		 velocidade permanece constante se obviamos
		 a forza de rozamento. Hai que ter en conta que
		 a grua pode xirar igualmente ainda que deixe de
		 acelerar. Multiplicamos pola matriz de xiro de
		 angulo angulo[1].*/
		tempvz = cos(angulo[1]) * moduloVelocidade();
		tempvx = sin(angulo[1]) * moduloVelocidade();
	}

	float rozamentox = rozamento * fabs(sin(angulo[1]));
	float rozamentoz = rozamento * fabs(cos(angulo[1]));

	// Limitamos o valor da velocidade para que non 
	// cambie de signo por culpa da forza de rozamento
	if (tempvx >= 0) {
		velocidade[0] = fmax(tempvx - rozamentox, 0);
	}
	else {
		velocidade[0] = fmin(tempvx + rozamentox, 0);
	}
	if (tempvz >= 0) {
		velocidade[2] = fmax(tempvz - rozamentoz, 0);
	}
	else {
		velocidade[2] = fmin(tempvz + rozamentoz, 0);
	}

	// Actualizamos a posicion da grua tendo en conta a velocidade
	posicion[0] = fmin(posicion[0] + velocidade[0], limitesPosicion[1]);
	posicion[0] = fmax(posicion[0] + velocidade[0], limitesPosicion[0]);
	posicion[2] = fmin(posicion[2] + velocidade[2], limitesPosicion[5]);
	posicion[2] = fmax(posicion[2] + velocidade[2], limitesPosicion[4]);
}


