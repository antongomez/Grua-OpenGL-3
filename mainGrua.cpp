
#include <glad.h>
#include <glfw3.h>
#include <stdio.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <math.h>

#include "esfera.h"
#include "lecturaShader_0_9.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define UNIDADE_GRAO_EN_RADIANS 0.0174533f

#define ACELERACION 0.004f


void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

extern GLuint setShaders(const char* nVertx, const char* nFrag);
GLuint shaderProgram;

unsigned int VAO, VAOCubo, VAOCadrado, VAOEsfera;

// Variables para controlar a camara
bool primeira = false;
bool terceira = false;
bool xeral = true;

// Para achegar, alonxar e xirar a cámara en vista xeral
float radio = 3.0f;
float alpha = M_PI / 2.0f;
float beta = M_PI / 4.0f;

float vx = 0;
float vz = 0;

float vxInicial = 0;
float vzInicial = 0;

float acceleration = 0; // Valor de aceleración
float rozamento = 0.00005f;
float deltaTime = 0.0f;    // Intervalo de tiempo desde la última actualización
float previousTime = 0;
bool empezar = true;

typedef struct {
	//Posicion inicial
	float px, py, pz;
	// Xiros da grua con respecto ao eixo y
	float angulo_x, angulo_y, angulo_z;
	// Escalado nos eixos
	float sx, sy, sz;
	// VAO
	unsigned int listaRender;
} obxecto;

obxecto base = { 0, 0.1f, 0, 0, 0, 0, 0.3f, 0.2f, 0.4f, 0 };
obxecto art1 = { 0, 0.05f, 0.0f, M_PI / 6.0f, 0, 0, 0.1f, 0.1f, 0.1f, 0 };
obxecto brazo1 = { 0, 0.35f, 0.0f, 0, 0, 0, 0.06f, 0.7f, 0.06f, 0 };
obxecto art2 = { 0, 0.35f, 0.0f, M_PI / 4.0f, 0, 0, 0.06f, 0.06f, 0.06f, 0 };
obxecto brazo2 = { 0, 0.2f, 0.0f, 0, 0, 0, 0.04f, 0.4f, 0.04f, 0 };

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

void vistaPrimeiraPersoa(float x, float y, float z, float angulo)
{
	float d2 = 1.0f, alturaOllos = 0.2, posicionSilla = 0.2;
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	// Matriz de vista
	glm::mat4 view;
	view = glm::mat4();
	view = glm::lookAt(
		glm::vec3(x + posicionSilla * sin(angulo), y + 0.15, z + posicionSilla * cos(angulo)),
		glm::vec3(x + d2 * sin(angulo), y + 0.15, z + d2 * cos(angulo)),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);
	unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	// Matriz de proxeccion
	glm::mat4 projection;
	projection = glm::mat4();
	projection = glm::perspective(
		glm::radians(60.0f),
		(float)SCR_WIDTH / (float)SCR_HEIGHT,
		0.1f, 10.0f
	);
	unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void vistaXeral(float x, float y, float z)
{
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	// Matriz de vista
	glm::mat4 view;
	view = glm::mat4();
	view = glm::lookAt(
		glm::vec3(radio * sin(alpha) * cos(beta), radio * sin(beta), radio * cos(alpha) * cos(beta)),
		glm::vec3(x, y, z),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);
	unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	// Matriz de proxeccion
	glm::mat4 projection;
	projection = glm::mat4();
	projection = glm::perspective(
		glm::radians(60.0f),
		(float)SCR_WIDTH / (float)SCR_HEIGHT,
		0.1f, 10.0f
	);
	unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void vistaTerceiraPersoa(float x, float y, float z, float angulo)
{
	float d1 = 0.6f, d2 = 3.0f;
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

	// Matriz de vista
	glm::mat4 view;
	view = glm::mat4();
	view = glm::lookAt(
		glm::vec3(x - d1 * sin(angulo), y + 0.4, z - d1 * cos(angulo)),
		glm::vec3(x + d2 * sin(angulo), y, z + d2 * cos(angulo)),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);
	unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	// Matriz de proxeccion
	glm::mat4 projection;
	projection = glm::mat4();
	projection = glm::perspective(
		glm::radians(60.0f),
		(float)SCR_WIDTH / (float)SCR_HEIGHT,
		0.1f, 10.0f
	);
	unsigned int projectionLoc = glGetUniformLocation(shaderProgram, "projection");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void debuxaEixos() {
	unsigned int VBO, EBO;
	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		//Vertices          //Colores
		0.0f, 0.0f, 0.0f,	 1.0f, 1.0f, 1.0f,  // 0
		2.0f, 0.0f, 0.0f,	 0.0f, 1.0f, 1.0f, //x
		0.0f, 2.0f, 0.0f,	 1.0f, 0.0f, 1.0f,// y
		0.0f, 0.0f, 2.0f,	 1.0f, 1.0f, 0.0f, // z  
		2.0f ,2.0f, 2.0f,	 1.0f, 1.0f, 1.0f // 1,1,1 bueno realmente la mitad
	};
	unsigned int indices[] = {  // empieza desde cero
		0, 1,
		0, 2,
		0, 3,
		0, 4
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// position Color

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);



	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

}

void debuxaCadrado() {
	unsigned int VBO;


	float vertices[] = {
		-0.5f, -0.5f,  0.0f,  0.0f, 1.0f,0.0f,
		 0.5f, -0.5f,  0.0f,  0.0f, 1.0f,0.0f,
		 0.5f,  0.5f,  0.0f, 0.0f, 1.0f,0.0f,

		 -0.5f, -0.5f,  0.0f,  0.0f, 1.0f,0.0f,
		 0.5f,  0.5f,  0.0f,  0.0f, 1.0f,0.0f,
		-0.5f,  0.5f,  0.0f,  0.0f, 1.0f,0.0f

	};

	glGenVertexArrays(1, &VAOCadrado);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAOCadrado);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Determinamos a posicion dos vertices no array
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Determinamos a posicion das cores no array
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDeleteBuffers(1, &VBO);

}

void debuxaCubo() {
	unsigned int VBO, EBO;


	float vertices[] = {
		-0.5f, -0.5f,  0.5f,  1.0f, 0.0f,0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f, 0.0f,1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,1.0f,

		-0.5f, -0.5f,  -0.5f, 1.0f, 0.0f,0.0f,
		 0.5f, -0.5f,  -0.5f, 1.0f, 0.0f,0.0f,
		 0.5f,  0.5f,  -0.5f, 0.0f, 0.0f,1.0f,
		-0.5f,  0.5f,  -0.5f, 0.0f, 1.0f,1.0f
	};

	unsigned int indices[] = {
		// Cara frontal
		0, 1, 2,
		0, 2, 3,

		// Cara de atrás
		4, 7, 6,
		4, 6, 5,

		// Cara superior
		2, 6, 3,
		3, 6, 7,

		// Cara inferior
		0, 4, 1,
		1, 4, 5,

		// Cara dereita
		1, 5, 2,
		5, 6, 2,

		// Cara esquerda
		0, 3, 4,
		4, 3, 7

	};

	glGenVertexArrays(1, &VAOCubo);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAOCubo);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// posicion cor
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void debuxaEsfera() {
	unsigned int VBO;

	extern float vertices_esfera[8640];

	glGenVertexArrays(1, &VAOEsfera);
	glGenBuffers(1, &VBO);
	// bind the Vertex Array Object first.
	glBindVertexArray(VAOEsfera);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_esfera), vertices_esfera, GL_STATIC_DRAW);

	// Vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Normais
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Texturas
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDeleteBuffers(1, &VBO);

}


void actualizarPosicion(float* x, float* z, float angulo) {
	float tempvx, tempvz;

	// No caso no que se esta pisando o acelerador
	if (acceleration > 0) {
		// Calcular a velocidade da grua en función da aceleración 
		// e do tempo que se leva pisando o acelerador
		tempvx = vxInicial + sin(angulo) * acceleration * deltaTime;
		tempvz = vzInicial + cos(angulo) * acceleration * deltaTime;
	}
	else { 
		// No caso no que deixamos de acelerar a 
		// velocidade permanece constante se obviamos 
		// a forza de rozamento
		tempvx = vx;
		tempvz = vz;
	}

	// Limitamos o valor da velocidade para que non 
	// cambie de signo por culpa da forza de rozamento
	if (tempvx >= 0) {
		vx = fmax(tempvx - rozamento, 0);
	}
	else {
		vx = fmin(tempvx + rozamento, 0);
	}
	if (tempvz >= 0) {
		vz = fmax(tempvz - rozamento, 0);
	}
	else {
		vz = fmin(tempvz + rozamento, 0);
	}

	// Actualizamos a posicion da grua tendo en conta a velocidade
	*x = fmin(*x + vx, 1.8);
	*x = fmax(*x + vx, -1.8);
	*z = fmin(*z + vz, 1.8);
	*z = fmax(*z + vz, -1.8);
}

void renderizarBase(int transformLoc, glm::mat4* transform, glm::mat4* transformTemp) {
	*transform = glm::mat4();
	*transform = glm::translate(*transform, glm::vec3(base.px, base.py, base.pz));
	*transform = glm::rotate(*transform, (float)base.angulo_y, glm::vec3(0.0, 1.0, 0.0));

	// Para mover o resto da grua coa base gardamos esta matriz para aplicarlla ao resto das componhentes
	*transformTemp = *transform;
	*transform = glm::scale(*transform, glm::vec3(base.sx, base.sy, base.sz));
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(*transform));
	glBindVertexArray(VAOCubo);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void renderizarArt1(int transformLoc, glm::mat4* transform, glm::mat4* transformTemp) {
	*transform = glm::mat4();
	*transform = glm::translate(*transform, glm::vec3(art1.px, art1.py, art1.pz));
	*transform = glm::rotate(*transform, (float)art1.angulo_x, glm::vec3(1.0, 0.0, 0.0));
	*transform = *transformTemp * (*transform);

	*transformTemp = *transform;

	*transform = glm::scale(*transform, glm::vec3(art1.sx, art1.sy, art1.sz));
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(*transform));
	glBindVertexArray(VAOEsfera);
	glDrawArrays(GL_TRIANGLES, 0, 1080);
}

void renderizarBrazo1(int transformLoc, glm::mat4* transform, glm::mat4* transformTemp) {
	*transform = *transformTemp;
	*transform = glm::translate(*transform, glm::vec3(brazo1.px, brazo1.py, brazo1.pz));

	*transformTemp = *transform;

	*transform = glm::scale(*transform, glm::vec3(brazo1.sx, brazo1.sy, brazo1.sz));
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(*transform));
	glBindVertexArray(VAOCubo);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void renderizarArt2(int transformLoc, glm::mat4* transform, glm::mat4* transformTemp) {
	*transform = glm::mat4();
	*transform = glm::translate(*transform, glm::vec3(art2.px, art2.py, art2.pz));
	*transform = glm::rotate(*transform, (float)art2.angulo_x, glm::vec3(1.0, 0.0, 0.0));
	*transform = *transformTemp * (*transform);


	*transformTemp = *transform;

	*transform = glm::scale(*transform, glm::vec3(art2.sx, art2.sy, art2.sz));
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(*transform));
	glBindVertexArray(VAOEsfera);
	glDrawArrays(GL_TRIANGLES, 0, 1080);
}

void renderizarBrazo2(int transformLoc, glm::mat4* transform, glm::mat4* transformTemp) {
	*transform = *transformTemp;
	*transform = glm::translate(*transform, glm::vec3(brazo2.px, brazo2.py, brazo2.pz));

	*transformTemp = *transform;

	*transform = glm::scale(*transform, glm::vec3(brazo2.sx, brazo2.sy, brazo2.sz));
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(*transform));
	glBindVertexArray(VAOCubo);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}



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
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Grua", NULL, NULL);
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

	debuxaCadrado();
	debuxaCubo();
	debuxaEixos();
	debuxaEsfera();

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
			vistaXeral(base.px, base.py, base.pz);
		}
		else if (terceira) {
			vistaTerceiraPersoa(base.px, base.py, base.pz, base.angulo_y);
		}
		else {
			vistaPrimeiraPersoa(base.px, base.py, base.pz, base.angulo_y);
		}

		glUseProgram(shaderProgram);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		// Matriz de model
		glm::mat4 transform;
		// Matriz de model para mover toda a grua ao mesmo tempo
		glm::mat4 transformTemp;
		// Buscamos a matriz no Shader
		unsigned int transformLoc = glGetUniformLocation(shaderProgram, "transform");

		// CHAN
		float i, j;
		float escala = 10.0;

		transform = glm::mat4();

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

		actualizarPosicion(&base.px, &base.pz, base.angulo_y);

		renderizarBase(transformLoc, &transform, &transformTemp);
		renderizarArt1(transformLoc, &transform, &transformTemp);
		renderizarBrazo1(transformLoc, &transform, &transformTemp);
		renderizarArt2(transformLoc, &transform, &transformTemp);
		renderizarBrazo2(transformLoc, &transform, &transformTemp);

		//EIXOS
		transform = glm::mat4();
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
		glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
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
	if (key == 68) {
		base.angulo_y -= UNIDADE_GRAO_EN_RADIANS;
	}

	// Tecla a: xiro da grua a esquerda
	if (key == 65) {
		base.angulo_y += UNIDADE_GRAO_EN_RADIANS;
	}

	// Tecla w: acelerador
	if (key == 87) {

		if (previousTime == 0) {
			previousTime = glfwGetTime();
			// Comezamos a acelerar (constante)
			acceleration = ACELERACION;
			// No caso de que a grua ainda estivera en movemento
			// tinha unha velocidade que hai que considerar 
			// e sumarlla a velocidade que se vai ganhar ao 
			// acelerar de novo
			vxInicial = vx;
			vzInicial = vz;
		}

		// Calcular el tiempo delta desde la última actualización
		deltaTime = glfwGetTime() - previousTime;

		// Producese cando se deixa de acelerar
		if (action == GLFW_RELEASE) {
			previousTime = 0;
			// Deixamos de acelerar
			acceleration = 0;
		}

	}

	// Tecla x: freno
	if (key == 88) {
		// Aumentamos o rozamento gradualmente
		rozamento += 0.00005f;

		// Producese cando se deixa de frear
		if (action == GLFW_RELEASE) {
			// Reestablecemos o valor do rozamento
			rozamento = 0.00005f;
		}
	}

	// Tecla r: subimos o primeiro brazo da grua
	if (key == 82) {
		art1.angulo_x = fmax(art1.angulo_x - UNIDADE_GRAO_EN_RADIANS, -M_PI / 5.0f);
	}

	// Tecla f: baixamos o primeiro brazo da grua
	if (key == 70) {
		art1.angulo_x = fmin(art1.angulo_x + UNIDADE_GRAO_EN_RADIANS, M_PI / 5.0f);
	}

	// Tecla t: subimos o segundo brazo da grua
	if (key == 84) {
		art2.angulo_x = fmax(art2.angulo_x - 2.0f * UNIDADE_GRAO_EN_RADIANS, -(2.0f * M_PI) / 3.0f);
	}

	// Tecla g: baixamos o segundo brazo da grua
	if (key == 71) {
		art2.angulo_x = fmin(art2.angulo_x + 2.0f * UNIDADE_GRAO_EN_RADIANS, (2.0f * M_PI) / 3.0f);
	}

	// Tecla dereita: xiro da camara en vistaXeral a dereita
	if (key == 262) {
		alpha += UNIDADE_GRAO_EN_RADIANS;
		if (alpha >= 2.0f * M_PI) {
			alpha -= 2.0f * M_PI;
		}
	}

	// So se esta activada a vista xeral podemos manexar a camara
	if (xeral) {
		// Tecla esquerda: xiro da camara en vistaXeral a esquerda
		if (key == 263) {
			alpha -= UNIDADE_GRAO_EN_RADIANS;
			if (alpha <= -2.0f * M_PI) {
				alpha += 2.0f * M_PI;
			}
		}

		// Tecla arriba: xiro da camara en vistaXeral na vertical cara arriba
		if (key == 265) {
			// Limite superior de 90 grados
			beta = fmin(beta + UNIDADE_GRAO_EN_RADIANS, M_PI / 2.0f - UNIDADE_GRAO_EN_RADIANS);
		}

		// Tecla abaixo: xiro da camara en vistaXeral na vertical cara abaixo
		if (key == 264) {
			// Limite superior de 90 grados
			beta = fmax(beta - UNIDADE_GRAO_EN_RADIANS, 0.0f + UNIDADE_GRAO_EN_RADIANS);
		}

		// Tecla +: achegase a camara en vistaXeral
		if (key == 93) {
			radio = fmax(radio - 0.1f, 1.0f);
		}

		// Tecla -: afastase a camara en vistaXeral
		if (key == 47) {
			radio = fmin(radio + 0.1f, 8.0f);
		}
	}

	// Tecla 1: ponse a vista en primeira persoa
	if (key == 49) {
		xeral = false;
		terceira = false;
		primeira = true;
	}
	// Tecla 2: ponse a vista en vista xeral
	else if (key == 50) {
		xeral = true;
		terceira = false;
		primeira = false;
	}
	// Tecla 3: ponse a vista en terceira persoa
	else if (key == 51) {
		xeral = false;
		terceira = true;
		primeira = false;
	}
}