/*
EJ08.01 - Simular varios objetos reales con sus materiales según la tabla en http://devernay.free.fr/cours/opengl/materials.html
(tener en cuenta que esos valores están calculados para luces blancas vec3(1.0)
*/

/*
De izquierda a derecha se dibujan esfera esmeralda, oro y goma verde
*/

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "engine/camera.hpp"
#include "engine/geometry/cube.hpp"
#include "engine/input.hpp"
#include "engine/shader.hpp"
#include "engine/texture.hpp"
#include "engine/window.hpp"
#include "engine/geometry/sphere.hpp"
#include "engine/geometry/quad.hpp"

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
glm::vec3 lightPos(4.0f, 1.0f, 0.0f);

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

void render(const Geometry& object, const Geometry& light, const Shader& s_phong, const Shader& s_light) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 proj = glm::perspective(glm::radians(camera.getFOV()), 800.0f / 600.0f, 0.1f, 100.0f);

    glm::vec3 lightDiffuse(1.0f, 1.0f, 1.0f);

    s_light.use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.25f));
    s_light.set("model", model);
    s_light.set("view", view);
    s_light.set("proj", proj);
    s_light.set("lightColor", lightDiffuse);

    light.render();

    //Generamos una primera esfera de material esmeralda
    s_phong.use();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
    s_phong.set("model", model);
    s_phong.set("view", view);
    s_phong.set("proj", proj);

    glm::mat3 normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
    s_phong.set("normalMat", normalMat);

    s_phong.set("viewPos", camera.getPosition());

    //Definimos material esmeralda
    s_phong.set("material.ambient", 0.0215f, 0.1745f, 0.0215f);
    s_phong.set("material.diffuse", 0.07568f, 0.61424f, 0.07568f);
    s_phong.set("material.specular", 0.633f, 0.727811f, 0.633f);
    s_phong.set("material.shininess", 0.6f*64);

    s_phong.set("light.position", lightPos);
    s_phong.set("light.ambient", 1.0f, 1.0f, 1.0f);
    s_phong.set("light.diffuse", lightDiffuse);
    s_phong.set("light.specular", 1.0f, 1.0f, 1.0f);

    object.render();

    //Generamos una segunda esfera de material oro
    s_phong.use();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
    s_phong.set("model", model);
    s_phong.set("view", view);
    s_phong.set("proj", proj);

    normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
    s_phong.set("normalMat", normalMat);

    s_phong.set("viewPos", camera.getPosition());

    //Definimos material oro
    s_phong.set("material.ambient", 0.24725f, 0.1995f, 0.0745f);
    s_phong.set("material.diffuse", 0.75164f, 0.60648f, 0.22648f);
    s_phong.set("material.specular", 0.628281f, 0.555802f, 0.366065f);
    s_phong.set("material.shininess", 0.4f*64);

    object.render();

    //Generamos una tercera esfera de material goma verde
    s_phong.use();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
    s_phong.set("model", model);
    s_phong.set("view", view);
    s_phong.set("proj", proj);

    normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
    s_phong.set("normalMat", normalMat);

    s_phong.set("viewPos", camera.getPosition());

    //Definimos material goma verde
    s_phong.set("material.ambient", 0.0f, 0.05f, 0.0f);
    s_phong.set("material.diffuse", 0.4f, 0.5f, 0.4f);
    s_phong.set("material.specular", 0.04f, 0.7f, 0.04f);
    s_phong.set("material.shininess", 0.078125f * 64);

    object.render();

}

int main(int, char* []) {
    Window* window = Window::instance();

    glClearColor(0.0f, 0.3f, 0.6f, 1.0f);

    const Shader s_phong("../projects/EJ08_01/phong.vs", "../projects/EJ08_01/blinn.fs");
    const Shader s_light("../projects/EJ08_01/light.vs", "../projects/EJ08_01/light.fs");
    const Sphere sphere(1.0f, 50, 50);

    Texture tex("../assets/textures/blue_blocks.jpg", Texture::Format::RGB);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

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
        render(sphere, sphere, s_phong, s_light);
        window->frame();
    }

    return 0;
}