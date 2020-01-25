/*
EJ13.01 - Usando una escena 3D cualquiera, pintarla de manera normal desde el punto de vista de una cámara.
Pero poner un panel en una esquina de la escena mostrando la misma escena desde el punto de vista de otra cámara.
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
#include <iostream>

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

Camera visor(glm::vec3(1.0f, 1.0f, 1.0f));

glm::vec3 lightPos(0.0f, 2.0f, 0.0f);

glm::vec3 cubePositions[] = {
    glm::vec3(4.0f, 0.0f, 0.0f),
    glm::vec3(-4.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, 4.0f),
    glm::vec3(0.0f, 0.0f, -4.0f),
    glm::vec3(4.0f, 0.0f, 4.0f),
    glm::vec3(4.0f, 0.0f, -4.0f),
    glm::vec3(-4.0f, 0.0f, 4.0f),
    glm::vec3(-4.0f, 0.0f, -4.0f),
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

std::tuple<uint32_t, uint32_t, uint32_t> createFBO() {
    uint32_t fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    uint32_t textureColor;
    glGenTextures(1, &textureColor);
    glBindTexture(GL_TEXTURE_2D, textureColor);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Window::instance()->getWidth(), Window::instance()->getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColor, 0);

    uint32_t rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, Window::instance()->getWidth(), Window::instance()->getHeight());
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer Incomplete" << std::endl;
    }

    return std::make_tuple(fbo, textureColor, rbo);
}

void renderVisor(const Geometry& floor, const Geometry& object, const Geometry& light, const Shader& s_phong, const Shader& s_light,
    const Texture& t_albedo, const Texture& t_specular, const uint32_t fbo, const uint32_t& fbo_texture) {

    glm::mat4 view = visor.getViewMatrix();
    glm::mat4 proj = glm::perspective(glm::radians(visor.getFOV()), static_cast<float>(Window::instance()->getWidth()) / static_cast<float>(Window::instance()->getHeight()), 0.1f, 100.0f);
    //visor.setYaw(glfwGetTime());

    //FIRST PASS
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    s_light.use();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.25f));
    s_light.set("model", model);
    s_light.set("view", view);
    s_light.set("proj", proj);
    s_light.set("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

    light.render();

    s_phong.use();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
    s_phong.set("model", model);
    s_phong.set("view", view);
    s_phong.set("proj", proj);

    glm::mat3 normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
    s_phong.set("normalMat", normalMat);

    s_phong.set("viewPos", camera.getPosition());

    s_phong.set("light.direction", -0.2f, -1.0f, -0.3f);
    s_phong.set("light.ambient", 0.1f, 0.1f, 0.1f);
    s_phong.set("light.diffuse", 0.5f, 0.5f, 0.5f);
    s_phong.set("light.specular", 1.0f, 1.0f, 1.0f);

    t_albedo.use(s_phong, "material.diffuse", 0);
    t_specular.use(s_phong, "material.specular", 1);
    s_phong.set("material.shininess", 32);

    floor.render();

    for (const auto& cubePos : cubePositions) {
        model = glm::mat4(1.0f);
        model = glm::translate(model, cubePos);
        s_phong.set("model", model);

        glm::mat3 normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
        s_phong.set("normalMat", normalMat);

        object.render();
    }
}

void renderCamera(const Geometry& floor, const Geometry& object, const Geometry& light,
    const Shader& s_phong, const Shader& s_light, const Shader& s_visor,
    const Texture& t_albedo, const Texture& t_specular, const uint32_t& fbo_texture) {
    
    //SECOND PASS
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 proj = glm::perspective(glm::radians(camera.getFOV()), static_cast<float>(Window::instance()->getWidth()) / static_cast<float>(Window::instance()->getHeight()), 0.1f, 100.0f);

    //Quad con textura
    s_visor.use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fbo_texture);
    s_visor.set("material.diffuse", 0);
    
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.6f, 0.6f, 0.0f));
    model = glm::scale(model, glm::vec3(0.6f, 0.6f, 1.0f));
    s_visor.set("model", model);
    s_visor.set("view", view);
    s_visor.set("proj", proj);

    glm::mat3 normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
    s_visor.set("normalMat", normalMat);

    s_visor.set("viewPos", camera.getPosition());

    floor.render();
    
    //Light
    s_light.use();
    model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.25f));
    s_light.set("model", model);
    s_light.set("view", view);
    s_light.set("proj", proj);
    s_light.set("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));

    light.render();

    //Scene
    s_phong.use();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
    s_phong.set("model", model);
    s_phong.set("view", view);
    s_phong.set("proj", proj);

    normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
    s_phong.set("normalMat", normalMat);

    s_phong.set("viewPos", camera.getPosition());

    s_phong.set("light.direction", -0.2f, -1.0f, -0.3f);
    s_phong.set("light.ambient", 0.1f, 0.1f, 0.1f);
    s_phong.set("light.diffuse", 0.5f, 0.5f, 0.5f);
    s_phong.set("light.specular", 1.0f, 1.0f, 1.0f);

    t_albedo.use(s_phong, "material.diffuse", 0);
    t_specular.use(s_phong, "material.specular", 1);
    s_phong.set("material.shininess", 32);

    floor.render();

    for (const auto& cubePos : cubePositions) {
        model = glm::mat4(1.0f);
        model = glm::translate(model, cubePos);
        s_phong.set("model", model);

        glm::mat3 normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
        s_phong.set("normalMat", normalMat);

        object.render();
    }
}

int main(int, char* []) {
    Window* window = Window::instance();

    glClearColor(0.0f, 0.3f, 0.6f, 1.0f);

    const Shader s_phong("../projects/EJ13_01/phong.vs", "../projects/EJ13_01/blinn.fs");
    const Shader s_visor("../projects/EJ13_01/visor.vs", "../projects/EJ13_01/visor.fs");
    const Shader s_light("../projects/EJ13_01/light.vs", "../projects/EJ13_01/light.fs");
    const Texture t_albedo("../assets/textures/bricks_albedo.png", Texture::Format::RGB);
    const Texture t_specular("../assets/textures/bricks_specular.png", Texture::Format::RGB);
    const Sphere sphere(1.0f, 50, 50);
    const Cube cube(1.0f);
    const Quad quad(1.0f);

    auto fbo = createFBO();

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
        renderCamera(quad, cube, sphere, s_phong, s_light, s_visor, t_albedo, t_specular, std::get<1>(fbo));
        renderVisor(quad, cube, sphere, s_phong, s_light, t_albedo, t_specular, std::get<0>(fbo), std::get<1>(fbo));
        window->frame();
    }

    return 0;
}