
#include <glad.h>
#include <glfw3.h>
#include <stdio.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Obxecto.h"
#include "ObxectoMobil.h"
#include "Camara.h"

#include "lecturaShader_0_9.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//#define UNIDADE_GRAO_EN_RADIANS 0.0174533f
// Realmente es un valor arbitrario para ajustar un movimiento fluido
#define UNIDADE_GRAO_EN_RADIANS 0.002f 

const unsigned int Camara::SCR_WIDTH = 800;
const unsigned int Camara::SCR_HEIGHT = 800;

// Posicion inicial
typedef struct {
	float px, py, pz;
} punto;

punto pview = { 0.0f, 0.0f, 0.0f };
punto pluz = { 0.0f, 0.0f, 0.0f };

// Variables para os VAOs
unsigned int VAO, VAOCubo, VAOCadrado, VAOEsfera;

// Variables para controlar a camara
bool primeira = false;
bool terceira = false;
bool xeral = true;

// velocidades iniciais da grua
float vxInicial = 0;
float vzInicial = 0;

// deltaTime. Intervalo de tempo desde a �ltima aceleracion
float deltaTime = 0.0f;
// previousTime. Momento no que se empezou a acelerar
float previousTime = 0;

// Creamos os obxectos que representan as distintas partes da grua
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

// Para pruebas
//Camara camara(3.0f, 0.0f, 0.0f);

// Flags para controles combinados
int giro_dcha = 0;
int giro_izda = 0;
int camara_arriba = 0;
int camara_abajo = 0;
int camara_izda = 0;
int camara_dcha = 0;

// Modo del foco
int modo = 0;

// Texturas
unsigned int texturaCesped;
unsigned int texturaPiscina[16];

// Animacion piscia
int instante = 0;
int contador = 0;


extern GLuint setShaders(const char* nVertx, const char* nFrag);
GLuint shaderProgram;

void processInput(GLFWwindow* window);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void teclasSimultaneas();

void openGlInit() {
	glClearDepth(1.0f); // Valor z-buffer
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);  // valor limpeza buffer cor
	glEnable(GL_DEPTH_TEST); // z-buffer
	glEnable(GL_CULL_FACE); // ocultacion caras back
	glCullFace(GL_BACK);
}

void renderizarChan(unsigned int transformLoc, glm::mat4* transform) {
	// CHAN
	float i, j;
	float escala = 3.0;

	for (i = -2; i <= 2; i += (float)(1.0 / escala)) {
		for (j = -2; j <= 2; j += (float)(1.0 / escala)) {
			*transform = glm::mat4();
			*transform = glm::rotate(*transform, (float)(-M_PI / 2.0), glm::vec3(1.0, 0.0, 0.0));
			*transform = glm::translate(*transform, glm::vec3(i, j, 0));
			*transform = glm::scale(*transform, glm::vec3(1.0 / escala, 1.0 / escala, 1.0 / escala));
			glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(*transform));
			glBindVertexArray(VAOCadrado);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
	}
}

void renderizarPiscina(unsigned int transformLoc, glm::mat4* transform) {
	// Piscina

		*transform = glm::mat4();
		*transform = glm::rotate(*transform, (float)(-M_PI / 2.0), glm::vec3(1.0, 0.0, 0.0));
		*transform = glm::translate(*transform, glm::vec3(1.0f, 0.0f, 0.01f));
		*transform = glm::scale(*transform, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(*transform));
		glBindVertexArray(VAOCadrado);
		glDrawArrays(GL_TRIANGLES, 0, 6);

}

void iluminacion_solo_ambiente() {
	// El color de la luz ambiente
	unsigned int lightLoc = glGetUniformLocation(shaderProgram, "lightColor");
	glUniform3f(lightLoc, 0.7f, 0.7f, 0.7f);

	// Flag para indicar que solo hay luz ambiente
	unsigned int soloAmbienteLoc = glGetUniformLocation(shaderProgram, "soloAmbiente");
	glUniform1i(soloAmbienteLoc, 1);
}

void iluminacion(glm::vec4 luz, glm::vec4 dir_luz) {

	// El color de la luz
	unsigned int lightLoc = glGetUniformLocation(shaderProgram, "lightColor");
	glUniform3f(lightLoc, 0.5f, 0.5f, 0.5f);

	// Luz difusa
	unsigned int lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
	glUniform3f(lightPosLoc, luz.x, luz.y, luz.z);

	// Luz especular (depende de la camara)
	glm::vec3 pos;
	if (xeral) {
		pos = camara.obtenerPosXeral();
	}
	else if (terceira) {
		pos = camara.obtenerPos3Pers(base.posicion[0], base.posicion[1], base.posicion[2], base.angulo[1]);
	}
	else {
		pos = camara.obtenerPos1Pers(base.posicion[0], base.posicion[1], base.posicion[2], base.angulo[1]);
	}
	unsigned int viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
	glUniform3f(viewPosLoc, pos.x, pos.y, pos.z);

	// Direccion de la luz
	unsigned int luzDirLoc = glGetUniformLocation(shaderProgram, "luzDir");
	//glUniform3f(luzDirLoc, 0.0f, -1.0f, 0.0f);
	glUniform3f(luzDirLoc, dir_luz.x, dir_luz.y, dir_luz.z);

	// Desactivamos el flag "soloAmbiente"
	unsigned int soloAmbienteLoc = glGetUniformLocation(shaderProgram, "soloAmbiente");
	glUniform1i(soloAmbienteLoc, 0);
}

void cargarTextura(const char* nombreTextura, unsigned int* textura, int formato) {

	// Cargamos y creamos la textura
	glGenTextures(1, textura);
	// Ahora todas las operaciones GL_TEXTURE_2D tienen efecto sobre este objeto texture
	glBindTexture(GL_TEXTURE_2D, *textura);
	// Establecemos los par�metros de wrapping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Establecemos los parametros de filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Cargamos la imagen, creamos la textura y generamos mipmaps
	int width, height, nrChannels;
	unsigned char* data = stbi_load(nombreTextura, &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, formato, width, height, 0, formato, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Error al cargar la textura" << std::endl;
	}
	stbi_image_free(data);

}

void texturasPiscina();
void texShader();
void activarTexturasPiscina();
void ajustarAnimacion();


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

	// Asignacion do contexto de OpenGL a venta
	glfwMakeContextCurrent(window);

	// glad: load all OpenGL function pointers
	// ---------------------------------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// Establecemento da funcion callback para cambiar o tamanho da venta
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Funcion que rexistra as pulsacions de teclado
	glfwSetKeyCallback(window, keyCallback);


	openGlInit();

	// Xeramos o Shaders
	shaderProgram = setShaders("shader.vert", "shader.frag");

	// Establecemos o shader que usa a camara
	camara.shaderProgram = shaderProgram;

	Obxecto::debuxaCadrado(&VAOCadrado);
	Obxecto::debuxaCubo(&VAOCubo);
	Obxecto::debuxaEixos(&VAO);
	Obxecto::debuxaEsfera(&VAOEsfera);

	// Obten a ubicaci�n das matrices de vista e proxeccion no programa de shader
	GLuint viewLoc = glGetUniformLocation(shaderProgram, "view");
	GLuint projectionLoc = glGetUniformLocation(shaderProgram, "projection");


	// Cargamos las texturas de la piscina
	texturasPiscina();

	// Cargamos la textura del cesped
	cargarTextura("texturas/cesped.jpg", &texturaCesped, GL_RGB);


	glUseProgram(shaderProgram);


	// Pasamos las texturas al shader
	texShader();
	
	// Habilitamos la mezcla de texturas
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

		// Colocamos a camara
		if (xeral) {
			camara.vistaXeral(base.posicion[0], base.posicion[1], base.posicion[2]);
		}
		else if (terceira) {
			camara.vistaTerceiraPersoa(base.posicion[0], base.posicion[1], base.posicion[2], base.angulo[1]);
		}
		else {
			camara.vistaPrimeiraPersoa(base.posicion[0], base.posicion[1], base.posicion[2], base.angulo[1]);
		}

		camara.actualizarMatrizProxeccion();

		glUseProgram(shaderProgram);	

		
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		// Matriz de model
		glm::mat4 transform = glm::mat4();
		// Matriz de model para mover toda a grua ao mesmo tempo
		glm::mat4 transformTemp = glm::mat4();
		// Buscamos a matriz de model no Shader
		unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");


		// TEXTURAS


		// Indicamos que para el suelo y la piscina utilizamos textura
		glUniform1i(glGetUniformLocation(shaderProgram, "usarTextura"), 1);


		// SUELO

		glActiveTexture(GL_TEXTURE16);
		glBindTexture(GL_TEXTURE_2D, texturaCesped);

		glUniform1i(glGetUniformLocation(shaderProgram, "piscina"), 0);

		renderizarChan(transformLoc, &transform);


		// PISCINA

		activarTexturasPiscina();

		glBindTexture(GL_TEXTURE_2D, texturaPiscina[instante]);
		glUniform1i(glGetUniformLocation(shaderProgram, "instante"), instante);

		// Ajustamos la animacion para que sea mas fluida
		ajustarAnimacion();
		

		glUniform1i(glGetUniformLocation(shaderProgram, "piscina"), 1);

		renderizarPiscina(transformLoc, &transform);

		base.actualizarPosicion(vxInicial, vzInicial, deltaTime);

		// Indicamos que para el resto de objetos no utilizamos textura
		glUniform1i(glGetUniformLocation(shaderProgram, "usarTextura"), 0);

		// Renderizamos todos los objetos que faltan
		base.renderizarObxecto(transformLoc, &transform, &transformTemp, VAOCubo);
		art1.renderizarObxecto(transformLoc, &transform, &transformTemp, VAOEsfera);
		brazo1.renderizarObxecto(transformLoc, &transform, &transformTemp, VAOCubo);
		art2.renderizarObxecto(transformLoc, &transform, &transformTemp, VAOEsfera);
		brazo2.renderizarObxecto(transformLoc, &transform, &transformTemp, VAOCubo);

		
		// ILUMINACION

		if (modo == 2) {	// Solo luz ambiente
			iluminacion_solo_ambiente();
		}
		else {		// Luz ambiente + foco

			// El brazo pequeno mide 0.4, de modo que la distancia del centro al extremo es 0.2
			glm::vec4 punta_brazo2 = glm::vec4(0.0f, 0.2f, 0.0f, 1.0f);

			glm::vec4 dir_luz;

			if (modo == 0) {
				// Para que la luz apunte perpendicular al brazo pequeno
				dir_luz = glm::vec4(0.0f, 0.2f, 1.0f, 1.0f);
			}
			else {
				// Para que la luz apunte en la misma direccion que el brazo pequeno
				dir_luz = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
			}

			punta_brazo2 = transformTemp * punta_brazo2;

			dir_luz = (transformTemp * dir_luz) - punta_brazo2;

			iluminacion(punta_brazo2, dir_luz);
		}

		//EIXOS
		/*transform = glm::mat4();
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
		glBindVertexArray(VAO);
		glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);*/

		teclasSimultaneas();

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


void texturasPiscina() {
	cargarTextura("texturas/caust00.png", &texturaPiscina[0], GL_RGBA);
	cargarTextura("texturas/caust01.png", &texturaPiscina[1], GL_RGBA);
	cargarTextura("texturas/caust02.png", &texturaPiscina[2], GL_RGBA);
	cargarTextura("texturas/caust03.png", &texturaPiscina[3], GL_RGBA);
	cargarTextura("texturas/caust04.png", &texturaPiscina[4], GL_RGBA);
	cargarTextura("texturas/caust05.png", &texturaPiscina[5], GL_RGBA);
	cargarTextura("texturas/caust06.png", &texturaPiscina[6], GL_RGBA);
	cargarTextura("texturas/caust07.png", &texturaPiscina[7], GL_RGBA);
	cargarTextura("texturas/caust08.png", &texturaPiscina[8], GL_RGBA);
	cargarTextura("texturas/caust09.png", &texturaPiscina[9], GL_RGBA);
	cargarTextura("texturas/caust10.png", &texturaPiscina[10], GL_RGBA);
	cargarTextura("texturas/caust11.png", &texturaPiscina[11], GL_RGBA);
	cargarTextura("texturas/caust12.png", &texturaPiscina[12], GL_RGBA);
	cargarTextura("texturas/caust13.png", &texturaPiscina[13], GL_RGBA);
	cargarTextura("texturas/caust14.png", &texturaPiscina[14], GL_RGBA);
	cargarTextura("texturas/caust15.png", &texturaPiscina[15], GL_RGBA);
}

void texShader() {
	glUniform1i(glGetUniformLocation(shaderProgram, "texturas[0]"), 0);
	glUniform1i(glGetUniformLocation(shaderProgram, "texturas[1]"), 1);
	glUniform1i(glGetUniformLocation(shaderProgram, "texturas[2]"), 2);
	glUniform1i(glGetUniformLocation(shaderProgram, "texturas[3]"), 3);
	glUniform1i(glGetUniformLocation(shaderProgram, "texturas[4]"), 4);
	glUniform1i(glGetUniformLocation(shaderProgram, "texturas[5]"), 5);
	glUniform1i(glGetUniformLocation(shaderProgram, "texturas[6]"), 6);
	glUniform1i(glGetUniformLocation(shaderProgram, "texturas[7]"), 7);
	glUniform1i(glGetUniformLocation(shaderProgram, "texturas[8]"), 8);
	glUniform1i(glGetUniformLocation(shaderProgram, "texturas[9]"), 9);
	glUniform1i(glGetUniformLocation(shaderProgram, "texturas[10]"), 10);
	glUniform1i(glGetUniformLocation(shaderProgram, "texturas[11]"), 11);
	glUniform1i(glGetUniformLocation(shaderProgram, "texturas[12]"), 12);
	glUniform1i(glGetUniformLocation(shaderProgram, "texturas[13]"), 13);
	glUniform1i(glGetUniformLocation(shaderProgram, "texturas[14]"), 14);
	glUniform1i(glGetUniformLocation(shaderProgram, "texturas[15]"), 15);
	glUniform1i(glGetUniformLocation(shaderProgram, "texturas[16]"), 16);
}

void activarTexturasPiscina() {
	switch (instante) {
	case 0:
		glActiveTexture(GL_TEXTURE0);
		break;
	case 1:
		glActiveTexture(GL_TEXTURE1);
		break;
	case 2:
		glActiveTexture(GL_TEXTURE2);
		break;
	case 3:
		glActiveTexture(GL_TEXTURE3);
		break;
	case 4:
		glActiveTexture(GL_TEXTURE4);
		break;
	case 5:
		glActiveTexture(GL_TEXTURE5);
		break;
	case 6:
		glActiveTexture(GL_TEXTURE6);
		break;
	case 7:
		glActiveTexture(GL_TEXTURE7);
		break;
	case 8:
		glActiveTexture(GL_TEXTURE8);
		break;
	case 9:
		glActiveTexture(GL_TEXTURE9);
		break;
	case 10:
		glActiveTexture(GL_TEXTURE10);
		break;
	case 11:
		glActiveTexture(GL_TEXTURE11);
		break;
	case 12:
		glActiveTexture(GL_TEXTURE12);
		break;
	case 13:
		glActiveTexture(GL_TEXTURE13);
		break;
	case 14:
		glActiveTexture(GL_TEXTURE14);
		break;
	case 15:
		glActiveTexture(GL_TEXTURE15);
		break;

	}
}

void ajustarAnimacion() {
	contador++;
	if (contador == 60) {
		if (instante == 10) {
			instante = 12; //Nos saltamos el 11 porque estropea la animacion
		}
		else if (instante < 15) {
			instante++;
		}
		else {
			instante = 0;
		}
		contador = 0;
	}
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// Implementaci�n de la funci�n de callback para cambiar el tama�o de la ventana
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	camara.width = width;
	camara.height = height;
	camara.actualizarMatrizProxeccion();
}


void teclasSimultaneas() {

	if (giro_dcha == 1) {
		base.angulo[1] -= UNIDADE_GRAO_EN_RADIANS;
	}

	if (giro_izda == 1) {
		base.angulo[1] += UNIDADE_GRAO_EN_RADIANS;
	}

	if (camara_arriba == 1) {
		camara.beta = fmin(camara.beta + UNIDADE_GRAO_EN_RADIANS, M_PI / 2.0f - UNIDADE_GRAO_EN_RADIANS);
	}

	if (camara_abajo == 1) {
		camara.beta = fmax(camara.beta - UNIDADE_GRAO_EN_RADIANS, 0.0f + UNIDADE_GRAO_EN_RADIANS);
	}

	if (camara_izda == 1) {
		camara.alpha -= UNIDADE_GRAO_EN_RADIANS;
		if (camara.alpha <= -2.0f * M_PI) {
			camara.alpha += 2.0f * M_PI;
		}
	}
	
	if (camara_dcha == 1) {
		camara.alpha += UNIDADE_GRAO_EN_RADIANS;
		if (camara.alpha >= 2.0f * M_PI) {
			camara.alpha -= 2.0f * M_PI;
		}
	}
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	std::cout << key << std::endl;

	// Tecla d: xiro da grua a dereita
	if (key == 68) {
		if (action != GLFW_RELEASE) {
			giro_dcha = 1;
			/*base.angulo[1] -= UNIDADE_GRAO_EN_RADIANS;*/
		}
		else {
			giro_dcha = 0;
		}
	}

	// Tecla a: xiro da grua a esquerda
	if (key == 65) {
		if (action != GLFW_RELEASE) {
			giro_izda = 1;
			/*base.angulo[1] += UNIDADE_GRAO_EN_RADIANS;*/
		}
		else {
			giro_izda = 0;
		}
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
			// Calcular o tiempo dende desde a ultima actualizaci�n
			deltaTime = glfwGetTime() - previousTime;
		}

	}

	// Tecla x: freno
	if (key == 88) {
		// Aumentamos o rozamento gradualmente
		base.rozamento += ACELERACION / 5.0f;

		// Producese cando se deixa de frear
		if (action == GLFW_RELEASE) {
			// Reestablecemos o valor do rozamento
			base.rozamento = ROZAMENTO;
		}
	}

	// Tecla t: subimos o primeiro brazo da grua
	if (key == 84 && action != GLFW_RELEASE) {
		art1.angulo[0] = fmax(art1.angulo[0] - 10*UNIDADE_GRAO_EN_RADIANS, -M_PI / 5.0f);
	}

	// Tecla g: baixamos o primeiro brazo da grua
	if (key == 71 && action != GLFW_RELEASE) {
		art1.angulo[0] = fmin(art1.angulo[0] + 10*UNIDADE_GRAO_EN_RADIANS, M_PI / 5.0f);
	}

	// Tecla f: xiramos a esquerda o primeiro brazo da grua
	if (key == 70 && action != GLFW_RELEASE) {
		art1.angulo[1] += 2.0f * 10 * UNIDADE_GRAO_EN_RADIANS;
	}

	// Tecla h: xiramos a dereita o primeiro brazo da grua
	if (key == 72 && action != GLFW_RELEASE) {
		art1.angulo[1] -= 2.0f * 10 * UNIDADE_GRAO_EN_RADIANS;
	}

	// Tecla i: subimos o segundo brazo da grua
	if (key == 73 && action != GLFW_RELEASE) {
		art2.angulo[0] = fmax(art2.angulo[0] - 2.0f * 10 * UNIDADE_GRAO_EN_RADIANS, -(2.0f * M_PI) / 3.0f);
	}

	// Tecla k: baixamos o segundo brazo da grua
	if (key == 75 && action != GLFW_RELEASE) {
		art2.angulo[0] = fmin(art2.angulo[0] + 2.0f * 10 * UNIDADE_GRAO_EN_RADIANS, (2.0f * M_PI) / 3.0f);
	}

	// Tecla j: xiramos a esquerda o primeiro brazo da grua
	if (key == 74 && action != GLFW_RELEASE) {
		art2.angulo[1] += 2.0f * 10 * UNIDADE_GRAO_EN_RADIANS;
	}

	// Tecla l: xiramos a dereita o primeiro brazo da grua
	if (key == 76 && action != GLFW_RELEASE) {
		art2.angulo[1] -= 2.0f * 10 * UNIDADE_GRAO_EN_RADIANS;
	}

	// Tecla dereita: xiro da camara en vistaXeral a dereita
	if (key == 262) {
		if (action != GLFW_RELEASE) {
			/*camara.alpha += UNIDADE_GRAO_EN_RADIANS;
			if (camara.alpha >= 2.0f * M_PI) {
				camara.alpha -= 2.0f * M_PI;
			}*/
			camara_dcha = 1;
		}
		else {
			camara_dcha = 0;
		}
	}

	// So se esta activada a vista xeral podemos manexar a camara
	if (xeral) {
		// Tecla esquerda: xiro da camara en vistaXeral a esquerda
		if (key == 263) {
			if (action != GLFW_RELEASE) {
				/*camara.alpha -= UNIDADE_GRAO_EN_RADIANS;
				if (camara.alpha <= -2.0f * M_PI) {
					camara.alpha += 2.0f * M_PI;
				}*/
				camara_izda = 1;
			}
			else {
				camara_izda = 0;
			}
		}

		// Tecla arriba: xiro da camara en vistaXeral na vertical cara arriba
		if (key == 265) {
			if (action != GLFW_RELEASE) {
				camara_arriba = 1;
				// Limite superior de 90 grados
				/*camara.beta = fmin(camara.beta + UNIDADE_GRAO_EN_RADIANS, M_PI / 2.0f - UNIDADE_GRAO_EN_RADIANS);*/
			}
			else {
				camara_arriba = 0;
			}
		}

		// Tecla abaixo: xiro da camara en vistaXeral na vertical cara abaixo
		if (key == 264) {
			if (action != GLFW_RELEASE) {
				camara_abajo = 1;
				// Limite superior de 90 grados
				/*camara.beta = fmax(camara.beta - UNIDADE_GRAO_EN_RADIANS, 0.0f + UNIDADE_GRAO_EN_RADIANS);*/
			}
			else {
				camara_abajo = 0;
			}
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

	// Tecla M: se cambia el modo de iluminacion
	if ((key == 77 || key == 109) && action == GLFW_PRESS) {
		if (modo == 0) {
			modo = 1;
		}
		else if (modo == 1) {
			modo = 2;
		}
		else {
			modo = 0;
		}
	}
}