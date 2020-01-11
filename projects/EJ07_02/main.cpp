/*
EJ07.02 - Cambiar valores de ambient, diffuse, specular y shininess y ver cómo se comporta el modelo de iluminación.
*/

/*
Se ha analizado el comportamiento en un archivo de PowerPoint adjunto en la carpeta en el que se han incluido imágenes 
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
glm::vec3 lightPos(3.0f, 1.0f, 2.0f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

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

void render(const Geometry& object, const Geometry& light, const Shader& s_blinn, const Shader& s_light) {

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 proj = glm::perspective(glm::radians(camera.getFOV()), 800.0f / 600.0f, 0.1f, 100.0f);


    s_light.use();

    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.25f));
    s_light.set("model", model);
    s_light.set("view", view);
    s_light.set("proj", proj);
    s_light.set("lightColor", lightColor);

    light.render();

    s_blinn.use();
    model = glm::mat4(1.0f);
    s_blinn.set("model", model);
    s_blinn.set("view", view);
    s_blinn.set("proj", proj);

    glm::mat3 normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
    s_blinn.set("normalMat", normalMat);

    s_blinn.set("objectColor", glm::vec3(0.6f, 0.5f, 0.2f));
    s_blinn.set("lightColor", lightColor);

    s_blinn.set("ambientStrength", 0.1f);
    s_blinn.set("lightPos", lightPos);

    //Factor diffuseStrength para controlar la cantidad de componente difusa
    s_blinn.set("diffuseStrength", 1.0f);

    s_blinn.set("viewPos", camera.getPosition());
    s_blinn.set("shininess", 64);
    s_blinn.set("specularStrength", 0.6f);

    object.render();
}

int main(int, char* []) {
    Window* window = Window::instance();

    glClearColor(0.0f, 0.3f, 0.6f, 1.0f);

    const Shader s_blinn("../projects/EJ07_02/phong.vs", "../projects/EJ07_02/blinn.fs");
    const Shader s_light("../projects/EJ07_02/light.vs", "../projects/EJ07_02/light.fs");
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
        render(sphere, sphere, s_blinn, s_light);
        window->frame();
    }

    return 0;
}