/*
EJ14.01 - Usando la misma escena de AG14, implementar las sombras que genera una fuente de luz de tipo SpotLight
(http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-16-shadow-mapping/ -> secci�n spotlights).
Se adjunta el codigo del fragment shader para visualizar el depth map para debug.
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
glm::vec3 lightPos(0.5f, 5.0f, 1.0f);
glm::vec3 lightDir(0.0f, -1.0f, 0.0f);

const uint32_t k_shadow_height = 1024;
const uint32_t k_shadow_width = 1024;
const float k_shadow_near = 1.0f;
const float k_shadow_far = 7.5f;

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

std::pair<uint32_t, uint32_t> createFBO() {
    uint32_t fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    uint32_t depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, k_shadow_width, k_shadow_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[]{ 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer Incomplete" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return std::make_pair(fbo, depthMap);
}

void renderScene(const Shader& shader1, const Shader& shader2, const Geometry& quad, const Geometry& cube, const Geometry& sphere,
    const Texture& t_albedo, const Texture& t_specular,
    glm::mat4 view, glm::mat4 proj) {
    t_albedo.use(shader1, "material.diffuse", 0);
    t_specular.use(shader1, "material.specular", 1);

    glm::mat4 model = glm::mat4(1.0);
    model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
    shader1.set("model", model);
    glm::mat3 normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
    shader1.set("normalMat", normalMat);
    quad.render();

    model = glm::mat4(1.0);
    shader1.set("model", model);
    normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
    shader1.set("normalMat", normalMat);
    cube.render();

    model = glm::mat4(1.0);
    model = glm::translate(model, glm::vec3(0.0f, 2.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
    shader1.set("model", model);
    normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
    shader1.set("normalMat", normalMat);
    sphere.render();

    //Light
    shader2.use();

    model = glm::mat4(1.0);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.25f));
    shader2.set("model", model);
    shader2.set("view", view);
    shader2.set("proj", proj);
    shader2.set("lightColor", 1.0f, 1.0f, 1.0f);
    sphere.render();
}

void render(const Geometry& quad, const Geometry& cube, const Geometry& sphere,
    const Shader& s_phong, const Shader& s_depth, const Shader& s_debug, const Shader& s_light,
    const Texture& t_albedo, const Texture& t_specular, const uint32_t fbo, const uint32_t fbo_texture) {

    //FIRST PASS
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, k_shadow_width, k_shadow_height);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    const glm::mat4 lightProjection = glm::perspective(glm::radians(45.0f), 1.0f, k_shadow_near, k_shadow_far);
    const glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::mat4 lightSpaceMatrix = lightProjection * lightView;

    s_depth.use();
    s_depth.set("lightSpaceMatrix", lightSpaceMatrix);
    //glCullFace(GL_FRONT);
    renderScene(s_depth, s_light, quad, cube, sphere, t_albedo, t_specular, lightView, lightProjection);
    //glCullFace(GL_BACK);

//SECOND PASS
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, Window::instance()->getWidth(), Window::instance()->getHeight());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 proj = glm::perspective(glm::radians(camera.getFOV()), static_cast<float>(Window::instance()->getWidth()) / Window::instance()->getHeight(), 0.1f, 100.0f);

    s_phong.use();

    s_phong.set("view", view);
    s_phong.set("proj", proj);

    s_phong.set("viewPos", camera.getPosition());

    s_phong.set("light.position", lightPos);
    s_phong.set("light.direction", lightDir);
    s_phong.set("light.ambient", 0.1f, 0.1f, 0.1f);
    s_phong.set("light.diffuse", 0.5f, 0.5f, 0.5f);
    s_phong.set("light.specular", 1.0f, 1.0f, 1.0f);
    s_phong.set("light.constant", 1.0f);
    s_phong.set("light.linear", 0.2f);
    s_phong.set("light.quadratic", 0.06f);
    s_phong.set("light.cutOff", glm::cos(glm::radians(30.0f)));
    s_phong.set("light.outerCutOff", glm::cos(glm::radians(35.0f)));


    s_phong.set("material.shininess", 32);

    s_phong.set("lightSpaceMatrix", lightSpaceMatrix);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, fbo_texture);
    s_phong.set("depthMap", 2);

    renderScene(s_phong, s_light, quad, cube, sphere, t_albedo, t_specular, view, proj);


    //Debug
    /*glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, Window::instance()->getWidth(), Window::instance()->getHeight());
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    s_debug.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fbo_texture);
    s_debug.set("depthMap", 0);
    s_debug.set("near_plane", k_shadow_near);
    s_debug.set("far_plane", k_shadow_far);

    quad.render();*/
}

int main(int, char* []) {
    Window* window = Window::instance();

    glClearColor(0.0f, 0.3f, 0.6f, 1.0f);

    const Shader s_phong("../projects/EJ14_01/phong.vs", "../projects/EJ14_01/blinn.fs");
    const Shader s_depth("../projects/EJ14_01/depth.vs", "../projects/EJ14_01/depth.fs");
    const Shader s_debug("../projects/EJ14_01/debug.vs", "../projects/EJ14_01/debug.fs");
    const Shader s_light("../projects/EJ14_01/light.vs", "../projects/EJ14_01/light.fs");
    const Texture t_albedo("../assets/textures/bricks_albedo.png", Texture::Format::RGB);
    const Texture t_specular("../assets/textures/bricks_specular.png", Texture::Format::RGB);
    const Quad quad(2.0f);
    const Cube cube(1.0f);
    const Sphere sphere(1.0f, 25, 25);

    auto fbo = createFBO();

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
        render(quad, cube, sphere, s_phong, s_depth, s_debug, s_light, t_albedo, t_specular, fbo.first, fbo.second);
        window->frame();
    }

    glDeleteFramebuffers(1, &fbo.first);
    glDeleteTextures(1, &fbo.second);

    return 0;
}