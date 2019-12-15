/*EJ04.04 - Pintar el quad y hacer que las coordenadas de textura hagan zoom sobre una parte de la textura,
y usar los metodos de filtrado para ver diferencias.
*/

/*Pulse las teclas "1" y "2" para intercambiar el tipo de Filtrado
1 --> Filtrado LINEAR
2 --> Filtrado NEAREST
*/

#include <glad/glad.h>

#include "engine/input.hpp"
#include "engine/window.hpp"
#include "engine/shader.hpp"

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void handleInput(uint32_t &PressedKey) {
	std::vector<std::pair<int, int>> keys = Input::instance()->getKeys();
	for (auto& key : keys) {
		std::cout << key.first << " - " << key.second << std::endl;
		if (key.first == 49 && key.second == 1) {
			PressedKey=1;
		}
		else if (key.first == 50 && key.second == 1) {
			PressedKey=2;
		}
	}
}

uint32_t createVertexData1(uint32_t* VBO, uint32_t* EBO) {
	float vertices[] = {
		0.5f, 0.5f, 0.0f,      1.0f, 0.0f, 0.0f,   0.7f, 0.7f,
		0.5f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f,   0.7f, 0.3f,
		-0.5f, -0.5f, 0.0f,    0.0f, 0.0f, 1.0f,   0.3f, 0.3f,
		-0.5f, 0.5f, 0.0f,     1.0f, 1.0f, 0.0f,   0.3f, 0.7f,
	};

	uint32_t indices[] = {
		0, 3, 1,
		1, 3, 2
	};

	uint32_t VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, VBO);
	glGenBuffers(1, EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, *VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return VAO;
}

//Quad para las instrucciones
uint32_t createVertexData2(uint32_t* VBO, uint32_t* EBO) {
	float vertices[] = {
		0.3f, -0.6f, 0.0f,      1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
		0.3f, -0.9f, 0.0f,     0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
		-0.3f, -0.9f, 0.0f,    0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
		-0.3f, -0.6f, 0.0f,     1.0f, 1.0f, 0.0f,   0.0f, 1.0f,
	};

	uint32_t indices[] = {
		0, 3, 1,
		1, 3, 2
	};

	uint32_t VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, VBO);
	glGenBuffers(1, EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, *VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 8, (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return VAO;
}

uint32_t createTexture(const char* path, uint32_t FilterOption) {
	uint32_t texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	if (FilterOption == 1) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	
	int width, height, nChannels;
	unsigned char* data = stbi_load(path, &width, &height, &nChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);
	}
	else {
		std::cout << "Error loading texture " << path << std::endl;
	}

	return texture;
}

void render(uint32_t VAO1, const Shader& shader1, uint32_t tex1, uint32_t VAO2, const Shader& shader2, uint32_t tex2) {
	glClear(GL_COLOR_BUFFER_BIT);

	shader1.use();
	glBindVertexArray(VAO1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex1);

	shader1.set("tex_1", 0);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	//Quad para las instrucciones
	shader2.use();
	glBindVertexArray(VAO2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex2);

	shader2.set("tex_1", 0);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

int main(int, char* []) {
	Window* window = Window::instance();

	stbi_set_flip_vertically_on_load(true);

	glClearColor(0.0f, 0.3f, 0.6f, 1.0f);

	uint32_t VBO1, EBO1;
	const uint32_t VAO1 = createVertexData1(&VBO1, &EBO1);
	const Shader shader1("../projects/EJ04_04/vertex.vs", "../projects/EJ04_04/fragment.fs");	

	//Quad para las instrucciones
	uint32_t VBO2, EBO2;
	const uint32_t VAO2 = createVertexData2(&VBO2, &EBO2);
	const Shader shader2("../projects/EJ04_04/vertex.vs", "../projects/EJ04_04/fragment.fs");

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	uint32_t tecla=1;

	while (window->alive()) {
		handleInput(tecla);

		uint32_t tex1 = createTexture("../assets/textures/wolf.jpg", tecla);
		uint32_t tex2 = createTexture("../assets/textures/instruccionesEJ04_04.png", tecla);

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);

		render(VAO1, shader1, tex1, VAO2, shader2, tex2);

		window->frame();

		glDeleteTextures(1, &tex1);
		glDeleteTextures(1, &tex2);
	}

	glDeleteVertexArrays(1, &VAO1);
	glDeleteBuffers(1, &VBO1);
	glDeleteBuffers(1, &EBO1);

	glDeleteVertexArrays(1, &VAO2);
	glDeleteBuffers(1, &VBO2);
	glDeleteBuffers(1, &EBO2);

	return 0;
}