/*
Con la escena anterior, poner el quad solido m�s alejado,
y los quads translucidos delante que actuen como un cristal de color.
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

Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
glm::vec3 lightPos(4.0f, 1.0f, 0.0f);

//Posici�n del Quad s�lido
glm::vec3 quadDiffusePosition = glm::vec3(0.0f, 0.0f, -4.0f);

//Posiciones, colores y transparencia de los Quads de color
glm::vec3 quadColorPositions[] = {
    glm::vec3(0.0f, 0.0f, -2.0f),
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, 2.0f),
};

glm::vec3 quadColor[] = {
    glm::vec3(1.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, 1.0f),
};

float alpha[]{
    0.25f,
    0.6,
    0.1,
};

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

void render(const Geometry& quad, const Shader& s_phong, const Shader& s_blend,
    const Texture& t_albedo, const Texture& t_specular, const Texture& t_albedo_wood, const Texture& t_specular_wood) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 proj = glm::perspective(glm::radians(camera.getFOV()), static_cast<float>(Window::instance()->getWidth()) / static_cast<float>(Window::instance()->getHeight()), 0.1f, 100.0f);


    //Suelo
    s_phong.use();
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
    s_phong.set("model", model);
    s_phong.set("view", view);
    s_phong.set("proj", proj);

    glm::mat3 normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
    s_phong.set("normalMat", normalMat);

    s_phong.set("viewPos", camera.getPosition());

    s_phong.set("light.direction", -0.2f, -0.1f, -0.3f);
    s_phong.set("light.ambient", 0.1f, 0.1f, 0.1f);
    s_phong.set("light.diffuse", 0.5f, 0.5f, 0.5f);
    s_phong.set("light.specular", 1.0f, 1.0f, 1.0f);

    t_albedo.use(s_phong, "material.diffuse", 0);
    t_specular.use(s_phong, "material.specular", 1);
    s_phong.set("material.shininess", 32);

    quad.render();

    //Quad s�lido
    s_phong.use();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(quadDiffusePosition));
    s_phong.set("model", model);
    s_phong.set("view", view);
    s_phong.set("proj", proj);

    normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
    s_phong.set("normalMat", normalMat);

    t_albedo_wood.use(s_phong, "material.diffuse", 0);
    t_specular_wood.use(s_phong, "material.specular", 1);
    s_phong.set("material.shininess", 32);

    quad.render();

    //Quads con colores transparentes
    for (int i = 0; i < 3; i++)
    {
        s_blend.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(quadColorPositions[i]));
        s_blend.set("model", model);
        s_blend.set("view", view);
        s_blend.set("proj", proj);

        normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
        s_blend.set("normalMat", normalMat);

        s_blend.set("viewPos", camera.getPosition());

        s_blend.set("light.direction", -0.2f, -0.1f, -0.3f);
        s_blend.set("light.ambient", 0.1f, 0.1f, 0.1f);
        s_blend.set("light.diffuse", 0.5f, 0.5f, 0.5f);
        s_blend.set("light.specular", 1.0f, 1.0f, 1.0f);

        s_blend.set("material.ambient", 0.1f * quadColor[i]);
        s_blend.set("material.diffuse", 0.5f * quadColor[i]);
        s_blend.set("material.specular", 1.0f * quadColor[i]);
        s_blend.set("material.shininess", 32);
        //Grado de transparencia para cada quad
        s_blend.set("material.alpha", alpha[i]);

        quad.render();
    }
}

int main(int, char* []) {
    Window* window = Window::instance();

    glClearColor(0.0f, 0.3f, 0.6f, 1.0f);

    const Shader s_phong("../projects/EJ11_03/phong.vs", "../projects/EJ11_03/blinn.fs");
    const Shader s_blend("../projects/EJ11_03/blend.vs", "../projects/EJ11_03/blend.fs");
    const Texture t_albedo("../assets/textures/bricks_albedo.png", Texture::Format::RGB);
    const Texture t_specular("../assets/textures/bricks_specular.png", Texture::Format::RGB);
    const Texture t_albedo_wood("../assets/textures/container2_diffuse.jpg", Texture::Format::RGB);
    const Texture t_specular_wood("../assets/textures/container2_specular.jpg", Texture::Format::RGB);
    const Quad quad(1.0f);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Input::instance()->setKeyPressedCallback(onKeyPress);
    Input::instance()->setMouseMoveCallback(onMouseMoved);
    Input::instance()->setScrollMoveCallback(onScrollMoved);

    while (window->alive()) {
        const float currentFrame = glfwGetTime();
        const float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        handleInput(deltaTime);
        render(quad, s_phong, s_blend, t_albedo, t_specular, t_albedo_wood, t_specular_wood);
        window->frame();
    }

    return 0;
}   