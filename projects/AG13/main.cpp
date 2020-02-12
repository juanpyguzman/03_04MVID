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
#include "engine/fbo.hpp"
#include <iostream>

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

FBO postProcessFBO(Window::instance()->getWidth(), Window::instance()->getHeight());

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

void render(const Geometry& quad, const Geometry& cube, const Shader& s_phong, const Shader& s_fbo,
    const Texture& t_albedo, const Texture& t_specular, const uint32_t fbo, const uint32_t fbo_texture) {
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 proj = glm::perspective(glm::radians(camera.getFOV()), static_cast<float>(Window::instance()->getWidth()) / Window::instance()->getHeight(), 0.1f, 100.0f);

    //FIRST PASS
    glEnable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    s_phong.use();
    s_phong.set("view", view);
    s_phong.set("proj", proj);

    s_phong.set("viewPos", camera.getPosition());

    s_phong.set("light.position", lightPos);
    s_phong.set("light.ambient", 0.1f, 0.0f, 0.0f);
    s_phong.set("light.diffuse", 0.5f, 0.0f, 0.0f);
    s_phong.set("light.specular", 1.0f, 0.0f, 0.0f);

    t_albedo.use(s_phong, "material.diffuse", 0);
    t_specular.use(s_phong, "material.specular", 1);
    s_phong.set("material.shininess", 32);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    s_phong.set("model", model);
    glm::mat3 normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
    s_phong.set("normalMat", normalMat);

    quad.render();

    model = glm::mat4(1.0f);
    s_phong.set("model", model);
    normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
    s_phong.set("normalMat", normalMat);

    cube.render();

    //SECOND PASS

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);

    glClear(GL_COLOR_BUFFER_BIT);

    s_fbo.use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fbo_texture);
    s_fbo.set("screenTexture", 0);

    quad.render();
}

int main(int, char* []) {
    Window* window = Window::instance();

    glClearColor(0.6f, 0.6f, 0.6f, 1.0f);

    const Shader s_phong("../projects/AG13/phong.vs", "../projects/AG13/blinn.fs");
    const Shader s_fbo("../projects/AG13/fbo.vs", "../projects/AG13/fbo.fs");
    const Texture t_albedo("../assets/textures/bricks_albedo.png", Texture::Format::RGB);
    const Texture t_specular("../assets/textures/bricks_specular.png", Texture::Format::RGB);
    const Quad quad(2.0f);
    const Cube cube(1.0f);

    auto fbo = postProcessFBO.createPostProcessFBO();

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    Input::instance()->setKeyPressedCallback(onKeyPress);
    Input::instance()->setMouseMoveCallback(onMouseMoved);
    Input::instance()->setScrollMoveCallback(onScrollMoved);

    while (window->alive()) {
        const float currentFrame = glfwGetTime();
        const float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        handleInput(deltaTime);
        render(quad, cube, s_phong, s_fbo, t_albedo, t_specular, std::get<0>(fbo), std::get<1>(fbo));
        window->frame();
    }

    glDeleteFramebuffers(1, &std::get<0>(fbo));
    glDeleteTextures(1, &std::get<1>(fbo));
    glDeleteRenderbuffers(1, &std::get<2>(fbo));

    return 0;
}