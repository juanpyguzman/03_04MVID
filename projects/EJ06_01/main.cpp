/*
EJ06.01 - Sobre la escena del ejercicio 05.04, hacer que la cámara no permita volar
y se comporte como una cámara FPS en la cual el personaje siempre estuviera andando sobre el suelo.
*/

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "engine/camera.hpp"
#include "engine/geometry/cube.hpp"
#include "engine/geometry/quad.hpp"
#include "engine/input.hpp"
#include "engine/shader.hpp"
#include "engine/texture.hpp"
#include "engine/window.hpp"

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastFrame = 0.0f;
float lastX, lastY;
bool firstMouse = true;

void handleInput(float dt) {
    Input* input = Input::instance();

    if (input->isKeyPressed(GLFW_KEY_W)) {
        camera.handleKeyboard(Camera::Movement::Forward, dt);
    }
    if (input->isKeyPressed(GLFW_KEY_S)) {
        camera.handleKeyboard(Camera::Movement::Backward, dt);
    }
    if (input->isKeyPressed(GLFW_KEY_A)) {
        camera.handleKeyboard(Camera::Movement::Left, dt);
    }
    if (input->isKeyPressed(GLFW_KEY_D)) {
        camera.handleKeyboard(Camera::Movement::Right, dt);
    }
}

void onKeyPress(int key, int action) {
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        Window::instance()->setCaptureMode(true);
    }

    if (key == GLFW_KEY_E && action == GLFW_PRESS) {
        Window::instance()->setCaptureMode(false);
    }
}

void onMouseMoved(float x, float y) {
    if (firstMouse) {
        firstMouse = false;
        lastX = x;
        lastY = y;
    }

    const float xoffset = x - lastX;
    const float yoffset = lastY - y;
    lastX = x;
    lastY = y;

    camera.handleMouseMovement(xoffset, yoffset);
}

void onScrollMoved(float x, float y) {
    camera.handleMouseScroll(y);
}

void render(const Geometry& geom1, const Shader& shader1, Texture& tex1, glm::vec3 positions1[], glm::vec1 sizes[], const Geometry& geom2, const Shader& shader2, Texture& tex2, glm::vec3 positions2) {

    //Activador de la opción de cámara tipo FPS.
    //FPS=1 activado.
    //FPS=0 desactivado.
    bool FPS = 1;

    //Liberamos los Buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Definimos las matrices model y proj
    glm::mat4 model = glm::mat4(1.0f);

    glm::mat4 proj = glm::perspective(glm::radians(camera.getFOV()), 800.0f / 600.0f, 0.1f, 100.0f);

    // Bucle para colocar los cubos
    for (uint8_t i = 0; i < 6; i++) {

        //Modificamos la matriz model para colocar, girar y escalar los cubos y los dibujamos

        // 1. Matriz de traslación
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(positions1[i]));

        //El cubo que está flotando se anima con un movimiento sinusoidal en el eje "y"
        if (i == 5)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(positions1[i].x, positions1[i].y + sin(static_cast<float>(glfwGetTime())), positions1[i].z));
        }

        // 2. Matriz de rotación
        model = glm::rotate(model, glm::radians(i * 30.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        //El cubo apoyado sobre otro cubo, lo animamos con un giro sobre el eje "y"
        if (i == 4)
        {
            model = glm::rotate(model, static_cast<float>(glfwGetTime())* glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        }

        // 3. Matriz de escalado
        model = glm::scale(model, sizes[i] * glm::vec3(1.0f, 1.0f, 1.0f));

        shader1.use();

        tex1.use(shader1, "tex1", 0);

        shader1.set("model", model);
        shader1.set("view", camera.getViewMatrix(FPS, positions2[1] + 1.0));
        shader1.set("proj", proj);

        geom1.render();

    }

    // Colocamos el suelo y lo dibujamos
    model = glm::mat4(1.0f);
    model = glm::translate(model, positions2);
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(-1.0f, 0.0f, 0.0f));

    shader2.use();

    tex2.use(shader2, "tex1", 0);

    shader2.set("model", model);
    shader2.set("view", camera.getViewMatrix(FPS, positions2[1]+1.0));
    shader2.set("proj", proj);

    geom2.render();
}

int main(int, char* []) {
    Window* window = Window::instance();

    glClearColor(0.0f, 0.3f, 0.6f, 1.0f);

    const Shader shader("../projects/EJ06_01/vertex.vs", "../projects/EJ06_01/fragment.fs");
 
    //Creamos los cubos y definimos sus posiciones
    const Cube cube(1.0f);

    glm::vec3 cubePositions[] = {
        //Cubos apoyados en el suelo
        glm::vec3(5.0f, -3.75f, -18.0f),
        glm::vec3(1.0f, -4.5f, -15.0f),
        glm::vec3(-3.8f, -4.0f, -12.3f),
        glm::vec3(-1.3f, -3.0f, -25.0f),

        //Cubo apoyado en el 4º cubo
        glm::vec3(-1.3f, -0.25f, -25.0f),

        //cubo flotando en el aire
        glm::vec3(2.0f, 3.0f, -20.0f),
    };

    glm::vec1 cubeSizes[] = {
        glm::vec1(2.5f),
        glm::vec1(1.0f),
        glm::vec1(2.0f),
        glm::vec1(4.0f),

        glm::vec1(1.5f),
        glm::vec1(2.0f),
    };

    //Creamos el suelo y definimos su posición
    const Quad suelo(45.0f);
    glm::vec3 sueloPosition = glm::vec3(0.0f, -5.0f, -20.0f);

    Texture tex("../assets/textures/blue_blocks.jpg", Texture::Format::RGB);
    Texture texsuelo("../assets/textures/bricks_albedo.png", Texture::Format::RGB);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    //Habilitamos y definimos la función del Buffer de Profundidad
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    Input::instance()->setKeyPressedCallback(onKeyPress);
    Input::instance()->setMouseMoveCallback(onMouseMoved);
    Input::instance()->setScrollMoveCallback(onScrollMoved);

    while (window->alive()) {
        const float currentFrame = glfwGetTime();
        const float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        handleInput(deltaTime);
        render(cube, shader, tex, cubePositions, cubeSizes, suelo, shader, texsuelo, sueloPosition);
        window->frame();
    }

    return 0;
}