
#include <glad.h>
#include <glfw3.h>
#include <stdio.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>

#include "Obxecto.h"
#include "ObxectoMobil.h"
#include "Camara.h"

#include "lecturaShader_0_9.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define UNIDADE_GRAO_EN_RADIANS 0.0174533f

const unsigned int Camara::SCR_WIDTH = 800;
const unsigned int Camara::SCR_HEIGHT = 800;

void processInput(GLFWwindow* window);

extern GLuint setShaders(const char* nVertx, const char* nFrag);
GLuint shaderProgram;

unsigned int VAO, VAOCubo, VAOCadrado, VAOEsfera;

// Variables para controlar a camara
bool primeira = false;
bool terceira = false;
bool xeral = true;

// velocidades iniciais da grua
float vxInicial = 0;
float vzInicial = 0;

// deltaTime. Intervalo de tempo desde a última aceleracion
float deltaTime = 0.0f;
// previousTime. Momento no que se empezou a acelerar
float previousTime = 0;

float posBase[] = { 0, 0.1f, 0 };
float escBase[] = { 0.3f, 0.2f, 0.4f };
float anguloBase[] = { 0, 0, 0 };
unsigned int vaoBase;
float limitesPosicion[] = { -1.8, 1.8, 0, 0, -1.8, 1.8 };
float velocidade[] = { 0, 0, 0 };
ObxectoMobil base(posBase, escBase, anguloBase, limitesPosicion, velocidade, 0);

float posArt1[] = { 0, 0.05f, 0 };
float anguloArt1[] = { M_PI / 6.0f, 0, 0 };
float escArt1[] = { 0.1f, 0.1f, 0.1f };
unsigned int vaoArt1;
ObxectoMobil art1(posArt1, escArt1, anguloArt1, 1);

float posBrazo1[] = { 0, 0.35f, 0 };
float escBrazo1[] = { 0.06f, 0.7f, 0.06f };
Obxecto brazo1(posBrazo1, escBrazo1, 0);

float posArt2[] = { 0, 0.35f, 0 };
float escArt2[] = { 0.06f, 0.06f, 0.06f };
float anguloArt2[] = { M_PI / 4.0f, 0, 0 };
ObxectoMobil art2(posArt2, escArt2, anguloArt2, 1);

float posBrazo2[] = { 0, 0.2f, 0 };
float escBrazo2[] = { 0.04f, 0.4f, 0.04f };
Obxecto brazo2(posBrazo2, escBrazo2, 0);

Camara camara(3.0f, M_PI / 2.0f, M_PI / 4.0f);

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

void openGlInit() {
	glClearDepth(1.0f); // Valor z-buffer
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // valor limpeza buffer cor
	glEnable(GL_DEPTH_TEST); // z-buffer
	glEnable(GL_CULL_FACE); // ocultacion caras back
	glCullFace(GL_BACK);
}


int main()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Creo a venta
	GLFWwindow* window = glfwCreateWindow(Camara::SCR_WIDTH, Camara::SCR_HEIGHT, "Grua", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	openGlInit();

	// xeramos o Shaders
	shaderProgram = setShaders("shader.vert", "shader.frag");

	// Creamos a camara
	camara.shaderProgram = shaderProgram;

	Obxecto::debuxaCadrado(&VAOCadrado);
	Obxecto::debuxaCubo(&VAOCubo);
	Obxecto::debuxaEixos(&VAO);
	Obxecto::debuxaEsfera(&VAOEsfera);

	// Obtén la ubicación de las matrices de vista y proyección en el programa de shader
	GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
	GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");

	// Funcion que rexistra as pulsacions de teclado
	glfwSetKeyCallback(window, keyCallback);

	// Lazo da venta mentres non se peche
	// -----------
	while (!glfwWindowShouldClose(window))
	{
		// input
		// -----
		processInput(window);

		// render
		// ------
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Coloco a camara
		if (xeral) {
			camara.vistaXeral(base.posicion[0], base.posicion[1], base.posicion[2]);
		}
		else if (terceira) {
			camara.vistaTerceiraPersoa(base.posicion[0], base.posicion[1], base.posicion[2], base.angulo[1]);
		}
		else {
			camara.vistaPrimeiraPersoa(base.posicion[0], base.posicion[1], base.posicion[2], base.angulo[1]);
		}

		glUseProgram(shaderProgram);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		// Matriz de model
		glm::mat4 transform = glm::mat4();
		// Matriz de model para mover toda a grua ao mesmo tempo
		glm::mat4 transformTemp = glm::mat4();
		// Buscamos a matriz no Shader
		unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");

		// CHAN
		float i, j;
		float escala = 10.0;

		for (i = -2; i <= 2; i += (float)(1.0 / escala)) {
			for (j = -2; j <= 2; j += (float)(1.0 / escala)) {
				transform = glm::mat4();
				transform = glm::rotate(transform, (float)(-M_PI / 2.0), glm::vec3(1.0, 0.0, 0.0));
				transform = glm::translate(transform, glm::vec3(i, j, 0));
				transform = glm::scale(transform, glm::vec3(1.0 / escala, 1.0 / escala, 1.0 / escala));
				glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
				glBindVertexArray(VAOCadrado);
				glDrawArrays(GL_TRIANGLES, 0, 6);
			}
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		base.actualizarPosicion(vxInicial, vzInicial, deltaTime);

		base.renderizarObxecto(transformLoc, &transform, &transformTemp, VAOCubo);
		art1.renderizarObxecto(transformLoc, &transform, &transformTemp, VAOEsfera);
		brazo1.renderizarObxecto(transformLoc, &transform, &transformTemp, VAOCubo);
		art2.renderizarObxecto(transformLoc, &transform, &transformTemp, VAOEsfera);
		brazo2.renderizarObxecto(transformLoc, &transform, &transformTemp, VAOCubo);

		//EIXOS
		transform = glm::mat4();
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
		glBindVertexArray(VAO);
		glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);


		// glfw: swap 
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &VAOCadrado);
	glDeleteVertexArrays(1, &VAOCubo);
	glDeleteVertexArrays(1, &VAOEsfera);

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	std::cout << key << std::endl;

	// Tecla d: xiro da grua a dereita
	if (key == 68 && action != GLFW_RELEASE) {
		base.angulo[1] -= UNIDADE_GRAO_EN_RADIANS;
	}

	// Tecla a: xiro da grua a esquerda
	if (key == 65 && action != GLFW_RELEASE) {
		base.angulo[1] += UNIDADE_GRAO_EN_RADIANS;
	}

	// Tecla w: acelerador
	if (key == 87) {

		if (action == GLFW_PRESS) {
			previousTime = glfwGetTime();
			// Comezamos a acelerar (constante)
			base.aceleracion = ACELERACION;
			// No caso de que a grua ainda estivera en movemento
			// tinha unha velocidade que hai que considerar 
			// e sumarlla a velocidade que se vai ganhar ao 
			// acelerar de novo
			vxInicial = base.velocidade[0];
			vzInicial = base.velocidade[2];
		}

		// Producese cando se deixa de acelerar
		if (action == GLFW_RELEASE) {
			// Deixamos de acelerar
			base.aceleracion = 0;
		}
		else {
			// Calcular o tiempo dende desde a ultima actualización
			deltaTime = glfwGetTime() - previousTime;
		}

	}

	// Tecla x: freno
	if (key == 88 && action != GLFW_RELEASE) {
		// Aumentamos o rozamento gradualmente
		base.rozamento += ACELERACION/5.0f;

		// Producese cando se deixa de frear
		if (action == GLFW_RELEASE) {
			// Reestablecemos o valor do rozamento
			base.rozamento = ROZAMENTO;
		}
	}

	// Tecla r: subimos o primeiro brazo da grua
	if (key == 82 && action != GLFW_RELEASE) {
		art1.angulo[0] = fmax(art1.angulo[0] - UNIDADE_GRAO_EN_RADIANS, -M_PI / 5.0f);
	}

	// Tecla f: baixamos o primeiro brazo da grua
	if (key == 70 && action != GLFW_RELEASE) {
		art1.angulo[0] = fmin(art1.angulo[0] + UNIDADE_GRAO_EN_RADIANS, M_PI / 5.0f);
	}

	// Tecla t: subimos o segundo brazo da grua
	if (key == 84 && action != GLFW_RELEASE) {
		art2.angulo[0] = fmax(art2.angulo[0] - 2.0f * UNIDADE_GRAO_EN_RADIANS, -(2.0f * M_PI) / 3.0f);
	}

	// Tecla g: baixamos o segundo brazo da grua
	if (key == 71 && action != GLFW_RELEASE) {
		art2.angulo[0] = fmin(art2.angulo[0] + 2.0f * UNIDADE_GRAO_EN_RADIANS, (2.0f * M_PI) / 3.0f);
	}

	// Tecla dereita: xiro da camara en vistaXeral a dereita
	if (key == 262 && action != GLFW_RELEASE) {
		camara.alpha += UNIDADE_GRAO_EN_RADIANS;
		if (camara.alpha >= 2.0f * M_PI) {
			camara.alpha -= 2.0f * M_PI;
		}
	}

	// So se esta activada a vista xeral podemos manexar a camara
	if (xeral) {
		// Tecla esquerda: xiro da camara en vistaXeral a esquerda
		if (key == 263 && action != GLFW_RELEASE) {
			camara.alpha -= UNIDADE_GRAO_EN_RADIANS;
			if (camara.alpha <= -2.0f * M_PI) {
				camara.alpha += 2.0f * M_PI;
			}
		}

		// Tecla arriba: xiro da camara en vistaXeral na vertical cara arriba
		if (key == 265 && action != GLFW_RELEASE) {
			// Limite superior de 90 grados
			camara.beta = fmin(camara.beta + UNIDADE_GRAO_EN_RADIANS, M_PI / 2.0f - UNIDADE_GRAO_EN_RADIANS);
		}

		// Tecla abaixo: xiro da camara en vistaXeral na vertical cara abaixo
		if (key == 264 && action != GLFW_RELEASE) {
			// Limite superior de 90 grados
			camara.beta = fmax(camara.beta - UNIDADE_GRAO_EN_RADIANS, 0.0f + UNIDADE_GRAO_EN_RADIANS);
		}

		// Tecla +: achegase a camara en vistaXeral
		if (key == 93 && action != GLFW_RELEASE) {
			camara.radio = fmax(camara.radio - 0.1f, 1.0f);
		}

		// Tecla -: afastase a camara en vistaXeral
		if (key == 47 && action != GLFW_RELEASE) {
			camara.radio = fmin(camara.radio + 0.1f, 8.0f);
		}
	}

	// Tecla 1: ponse a vista en primeira persoa
	if (key == 49 && action == GLFW_PRESS) {
		xeral = false;
		terceira = false;
		primeira = true;
	}
	// Tecla 2: ponse a vista en vista xeral
	else if (key == 50 && action == GLFW_PRESS) {
		xeral = true;
		terceira = false;
		primeira = false;
	}
	// Tecla 3: ponse a vista en terceira persoa
	else if (key == 51 && action == GLFW_PRESS) {
		xeral = false;
		terceira = true;
		primeira = false;
	}
}