#include "Obxecto.h"
#include "esfera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad.h>

Obxecto::Obxecto(float* posicion, float* escalado, int tipo) {
	for (int i = 0; i < 3; i++) {
		this->posicion[i] = posicion[i];
		this->escalado[i] = escalado[i];
	}

	this->tipo = tipo;
}

void Obxecto::debuxaEixos(unsigned int* VAO) {
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

	glGenVertexArrays(1, VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(*VAO);

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

void Obxecto::debuxaCadrado(unsigned int* VAO) {
	unsigned int VBO;


	float vertices[] = {
		-0.5f, -0.5f,  0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
		 0.5f, -0.5f,  0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f,  0.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 1.0f,

		 -0.5f, -0.5f,  0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
		 0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
		-0.5f,  0.5f,  0.0f,  0.0f, 1.0f, 0.0f,  0.0f, .0f, 1.0f

	};

	glGenVertexArrays(1, VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(*VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Determinamos a posicion dos vertices no array
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Determinamos a posicion das cores no array
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Determinamos a posicion dos vectores normais no array
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDeleteBuffers(1, &VBO);

}

void Obxecto::debuxaCubo(unsigned int* VAO) {
	unsigned int VBO, EBO;

	float n = 0.577350f;	// Este valor es 1/sqrt(3)

	float vertices[] = {
		-0.5f, -0.5f,  0.5f,  1.0f, 0.0f,0.0f,  -n, -n, n,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,0.0f,  n, -n, n,
		 0.5f,  0.5f,  0.5f,  0.0f, 0.0f,1.0f,  n, n, n,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,1.0f,  -n, n, n,

		-0.5f, -0.5f,  -0.5f, 1.0f, 0.0f,0.0f,  -n, -n, -n,
		 0.5f, -0.5f,  -0.5f, 1.0f, 0.0f,0.0f,  n, -n, -n,
		 0.5f,  0.5f,  -0.5f, 0.0f, 0.0f,1.0f,  n, n, -n,
		-0.5f,  0.5f,  -0.5f, 0.0f, 1.0f,1.0f,  -n, n, -n
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

	glGenVertexArrays(1, VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(*VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// cor
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// normais
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void Obxecto::debuxaEsfera(unsigned int* VAO) {
	unsigned int VBO;

	extern float vertices_esfera[8640];

	glGenVertexArrays(1, VAO);
	glGenBuffers(1, &VBO);
	// bind the Vertex Array Object first.
	glBindVertexArray(*VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_esfera), vertices_esfera, GL_STATIC_DRAW);

	// Vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(0);

	// Normais
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(2);

	// Texturas
	// MIENTRAS NO METAMOS TEXTURAS, EL SHADER INTERPRETA ESTO COMO COLOR
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glDeleteBuffers(1, &VBO);

}

// Funcions para debuxar as distintas partes da grua

void Obxecto::renderizarObxecto(int transformLoc, glm::mat4* transform, glm::mat4* transformTemp, unsigned int VAO) {
	*transform = glm::mat4();
	*transform = *transformTemp;
	*transform = glm::translate(*transform, glm::vec3(this->posicion[0], this->posicion[1], this->posicion[2]));
	rotarObxecto(transform, transformTemp);

	// Para mover o resto da grua coa base gardamos esta matriz para aplicarlla ao resto das componhentes
	*transformTemp = *transform;

	*transform = glm::scale(*transform, glm::vec3(this->escalado[0], this->escalado[1], this->escalado[2]));
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(*transform));
	if (tipo == 0) {
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	}
	else {
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 1080);
	}
}

void Obxecto::rotarObxecto(glm::mat4* transform, glm::mat4* transformTemp) {} // Non fai nada


