/*EJ04.05 - Pintar el quad con dos texturas interpoladas, y controlar el valor de la interpolación entre ambas con el teclado.
*/

/*Pulse las teclas + y - para variar la interpolación*/

#include <glad/glad.h>

#include "engine/input.hpp"
#include "engine/window.hpp"
#include "engine/shader.hpp"

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void handleInput(int32_t &counter) {
	std::vector<std::pair<int, int>> keys = Input::instance()->getKeys();
	for (auto& key : keys) {
		std::cout << key.first << " - " << key.second << std::endl;
		if (key.first==334 && key.second==1){
			counter++;
		}
		else if (key.first==333 && key.second==1){
			counter--;
		}
		else {
			counter = counter;
		}
			}
}

uint32_t createVertexData1(uint32_t* VBO, uint32_t* EBO) {
	float vertices[] = {
		0.5f, 0.5f, 0.0f,      1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
		0.5f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
		-0.5f, -0.5f, 0.0f,    0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,     1.0f, 1.0f, 0.0f,   0.0f, 1.0f,
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

uint32_t createTexture(const char* path) {
	uint32_t texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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

void render(uint32_t VAO1, const Shader& shader1, uint32_t tex1, uint32_t tex2, int32_t counter, uint32_t VAO2, const Shader& shader2, uint32_t tex3) {
	glClear(GL_COLOR_BUFFER_BIT); 

	shader1.use();
	shader1.set("counter", static_cast<int>(counter));

	glBindVertexArray(VAO1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex1);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, tex2);

	shader1.set("tex_1", 0);
	shader1.set("tex_2", 1);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

	//Quad para las instrucciones
	shader2.use();

	glBindVertexArray(VAO2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex3);

	shader2.set("tex_1", 0);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

}

int main(int, char* []) {
	Window* window = Window::instance();

	stbi_set_flip_vertically_on_load(true);

	glClearColor(0.0f, 0.3f, 0.6f, 1.0f);

	uint32_t VBO1, EBO1;
	const uint32_t VAO1 = createVertexData1(&VBO1, &EBO1);
	const Shader shader1("../projects/EJ04_05/vertex.vs", "../projects/EJ04_05/fragment.fs");

	uint32_t tex1 = createTexture("../assets/textures/blue_blocks.jpg");
	uint32_t tex2 = createTexture("../assets/textures/led_zeppelin_logo.jpg");

	//Quad para las instrucciones
	uint32_t VBO2, EBO2;
	const uint32_t VAO2 = createVertexData2(&VBO2, &EBO2);
	const Shader shader2("../projects/EJ04_05/vertex.vs", "../projects/EJ04_05/fragment.fs");

	uint32_t tex3 = createTexture("../assets/textures/instruccionesEJ04_05.jpg");
	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	int32_t contador=0;

	while (window->alive()) {
		handleInput(contador);
		render(VAO1, shader1, tex1, tex2, contador, VAO2, shader2, tex3);
		window->frame();
	}

	glDeleteVertexArrays(1, &VAO1);
	glDeleteBuffers(1, &VBO1);
	glDeleteBuffers(1, &EBO1);

	glDeleteVertexArrays(1, &VAO2);
	glDeleteBuffers(1, &VBO2);
	glDeleteBuffers(1, &EBO2);

	glDeleteTextures(1, &tex1);
	glDeleteTextures(1, &tex2);
	glDeleteTextures(1, &tex3);


	return 0;
}